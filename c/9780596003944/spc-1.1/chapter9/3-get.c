SSL_SESSION *spc_getsession(BIO *conn) {
  SSL *ssl_ptr;

  BIO_get_ssl(conn, &ssl_ptr);
  if (!ssl_ptr) return 0;
  return SSL_get1_session(ssl_ptr);
}
