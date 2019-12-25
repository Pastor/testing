static void *retrieve_webdata(char *uri, int *datalen, spc_x509store_t *store) {
  int     bytes, content_length = 0, headerlen = 0, sd, ssl;
  BIO     *conn = 0;
  SSL     *ssl_ptr;
  char    buffer[1024];
  char    *headers = 0, *host = 0, *path = 0, *port = 0, *tmp;
  void    *data = 0;
  fd_set  rmask, wmask;
  SSL_CTX *ctx = 0;

  *datalen = 0;
  if (!OCSP_parse_url(uri, &host, &port, &path, &ssl)) goto end_error;
  if (!(conn = spc_connect(host, atoi(port), ssl, store, &ctx))) goto end_error;

  /* Send the request for the data */
  BIO_printf(conn, "GET %s HTTP/1.0\r\nConnection: close\r\n\r\n", path);

  /* Put the socket into non-blocking mode */
  BIO_get_fd(conn, &sd);
  BIO_socket_nbio(sd, 1);
  if (ssl) {
    BIO_get_ssl(conn, &ssl_ptr);
    SSL_set_mode(ssl_ptr, SSL_MODE_ENABLE_PARTIAL_WRITE |
                          SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
  }

  /* Loop reading data from the socket until we've got all of the headers */
  for (;;) {
    FD_ZERO(&rmask);
    FD_SET(sd, &rmask);
    FD_ZERO(&wmask);
    if (BIO_should_write(conn)) FD_SET(sd, &wmask);
    if (select(FD_SETSIZE, &rmask, &wmask, 0, 0) <= 0) continue;
    if (FD_ISSET(sd, &wmask)) BIO_write(conn, buffer, 0);
    if (FD_ISSET(sd, &rmask)) {
      if ((bytes = BIO_read(conn, buffer, sizeof(buffer))) <= 0) {
        if (BIO_should_retry(conn)) continue;
        goto end_error;
      }
      if (!(headers = (char *)realloc((tmp = headers), headerlen + bytes))) {
        headers = tmp;
        goto end_error;
      }
      memcpy(headers + headerlen, buffer, bytes);
      headerlen += bytes;
      if ((tmp = strstr(headers, "\r\n\r\n")) != 0) {
        *(tmp + 2) = '\0';
        *datalen = headerlen - ((tmp + 4) - headers);
        headerlen -= (*datalen + 2);
        if (*datalen > 0) {
          if (!(data = (char *)malloc(*datalen))) goto end_error;
          memcpy(data, tmp + 4, *datalen);
        }
        break;
      }
    }
  }

  /* Examine the headers to determine whether or not to continue.  If we are to
   * continue, look for a content-length header to find out how much data we're
   * going to get.  If there is no content-length header, we'll have to read
   * until the remote server closes the connection.
   */
  if (!strncasecmp(headers, "HTTP/1.", 7)) {
    if (!(tmp = strchr(headers, ' '))) goto end_error;
    if (strncmp(tmp + 1, "200 ", 4) && strncmp(tmp + 1, "200\r\n", 5))
      goto end_error;
    for (tmp = strstr(headers, "\r\n");  tmp;  tmp = strstr(tmp + 2, "\r\n")) {
      if (strncasecmp(tmp + 2, "content-length: ", 16)) continue;
      content_length = atoi(tmp + 18);
      break;
    }
  } else goto end_error;

  /* Continuously read and accumulate data from the remote server.  Finish when
   * we've read up to the content-length that we received. If we didn't receive
   * a content-length, read until the remote server closes the connection.
   */
  while (!content_length || *datalen < content_length) {
    FD_ZERO(&rmask);
    FD_SET(sd, &rmask);
    FD_ZERO(&wmask);
    if (BIO_should_write(conn)) FD_SET(sd, &wmask);
    if (select(FD_SETSIZE, &rmask, &wmask, 0, 0) <= 0) continue;
    if (FD_ISSET(sd, &wmask)) BIO_write(conn, buffer, 0);
    if (FD_ISSET(sd, &rmask))
      if ((bytes = BIO_read(conn, buffer, sizeof(buffer))) <= 0) {
        if (BIO_should_retry(conn)) continue;
        break;
      }
    if (!(data = realloc((tmp = data), *datalen + bytes))) {
      data = tmp;
      goto end_error;
    }
    memcpy((char *)data + *datalen, buffer, bytes);
    *datalen += bytes;
  }

  if (content_length && *datalen != content_length) goto end_error;
  goto end;

end_error:
  if (data) { free(data);  data = 0;  *datalen = 0; }
end:
  if (headers) free(headers);
  if (conn) BIO_free_all(conn);
  if (host) OPENSSL_free(host);
  if (port) OPENSSL_free(port);
  if (path) OPENSSL_free(path);
  if (ctx) SSL_CTX_free(ctx);
  return data;
}
