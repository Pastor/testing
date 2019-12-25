#include <openssl/x509.h>

int spc_verify_cert(X509 *cert, spc_x509store_t *spc_store) {
  int            result = -1;
  X509_STORE     *store = 0;
  X509_STORE_CTX *ctx = 0;

  if (!(store = spc_create_x509store(spc_store))) return -1;
  if ((ctx = X509_STORE_CTX_new()) != 0) {
    if (X509_STORE_CTX_init(ctx, store, cert, 0) == 1)
      result = (X509_verify_cert(ctx) == 1);
    X509_STORE_CTX_free(ctx);
  }
  X509_STORE_free(store);
  return result;
}
