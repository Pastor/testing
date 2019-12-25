BIO *spc_connect(char *host, int port, int ssl, spc_x509store_t *spc_store,
                 SSL_CTX **ctx) {
  BIO *conn;
  SSL *ssl_ptr;

  if (ssl) {
    if (!(conn = spc_connect_ssl(host, port, spc_store, ctx))) goto error_exit;
    BIO_get_ssl(conn, &ssl_ptr);
    if (!spc_verify_cert_hostname(SSL_get_peer_certificate(ssl_ptr), host))
      goto error_exit;
    if (SSL_get_verify_result(ssl_ptr) != X509_V_OK) goto error_exit;
    return conn;
  }

  *ctx = 0;
  if (!(conn = BIO_new_connect(host))) goto error_exit;
  BIO_set_conn_int_port(conn, &port);
  if (BIO_do_connect(conn) <= 0) goto error_exit;
  return conn;

error_exit:
  if (conn) BIO_free_all(conn);
  return 0;
}
