#include <openssl/ocsp.h>
#include <openssl/ssl.h>

typedef struct {
  char            *url;
  X509            *cert;
  X509            *issuer;
  spc_x509store_t *store;
  X509            *sign_cert;
  EVP_PKEY        *sign_key;
  long            skew;
  long            maxage;
} spc_ocsprequest_t;
