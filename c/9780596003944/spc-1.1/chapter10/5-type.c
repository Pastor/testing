#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/x509.h>

typedef int (*spc_x509verifycallback_t)(int, X509_STORE_CTX *);

typedef struct {
  char                      *cafile;
  char                      *capath;
  char                      *crlfile;
  spc_x509verifycallback_t  callback;
  STACK_OF(X509)            *certs;
  STACK_OF(X509_CRL)        *crls;
  char                      *use_certfile;
  STACK_OF(X509)            *use_certs;
  char                      *use_keyfile;
  EVP_PKEY                  *use_key;
  int                       flags;
} spc_x509store_t;
