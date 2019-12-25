BIO *spc_accept(BIO *parent, int ssl, spc_x509store_t *spc_store, SSL_CTX **ctx) {
  BIO *child = 0, *ssl_bio = 0;
  int our_ctx = 0;
  SSL *ssl_ptr = 0;

  if (BIO_do_accept(parent) <= 0) return 0;
  if (!(child = BIO_pop(parent))) return 0;

  if (ssl) {
    if (*ctx) {
      CRYPTO_add(&((*ctx)->references), 1, CRYPTO_LOCK_SSL_CTX);
      if (spc_store && spc_store != SSL_CTX_get_app_data(*ctx)) {
        SSL_CTX_set_cert_store(*ctx, spc_create_x509store(spc_store));
        SSL_CTX_set_app_data(*ctx, spc_store);
      }
    } else {
      *ctx = spc_create_sslctx(spc_store);
      our_ctx = 1;
    }

    if (!(ssl_ptr = SSL_new(*ctx))) goto error_exit;
    SSL_set_bio(ssl_ptr, child, child);
    if (SSL_accept(ssl_ptr) <= 0) goto error_exit;

    if (!(ssl_bio = BIO_new(BIO_f_ssl()))) goto error_exit;
    BIO_set_ssl(ssl_bio, ssl_ptr, 1);
    child = ssl_bio;
    ssl_bio = 0;
  }

  return child;

error_exit:
  if (child) BIO_free_all(child);
  if (ssl_bio) BIO_free_all(ssl_bio);
  if (ssl_ptr) SSL_free(ssl_ptr);
  if (*ctx) SSL_CTX_free(*ctx);
  if (our_ctx) *ctx = 0;
  return 0;
}
