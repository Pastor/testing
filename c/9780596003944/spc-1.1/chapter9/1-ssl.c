#include <openssl/bio.h>
#include <openssl/ssl.h>

BIO *spc_connect_ssl(char *host, int port, spc_x509store_t *spc_store,
                     SSL_CTX **ctx) {
  BIO *conn = 0;
  int our_ctx = 0;

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

  if (!(conn = BIO_new_ssl_connect(*ctx))) goto error_exit;
  BIO_set_conn_hostname(conn, host);
  BIO_set_conn_int_port(conn, &port);

  if (BIO_do_connect(conn) <= 0) goto error_exit;
  if (our_ctx) SSL_CTX_free(*ctx);
  return conn;

error_exit:
  if (conn) BIO_free_all(conn);
  if (*ctx) SSL_CTX_free(*ctx);
  if (our_ctx) *ctx = 0;
  return 0;
}
