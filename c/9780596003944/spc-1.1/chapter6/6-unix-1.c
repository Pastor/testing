#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>

/* Returns 0 when malloc() fails. */
unsigned char *spc_digest_message(EVP_MD *type, unsigned char *in,
                                  unsigned long n, unsigned int *outlen) {
  EVP_MD_CTX    ctx;
  unsigned char *ret;

  EVP_DigestInit(&ctx, type);
  EVP_DigestUpdate(&ctx, in, n);
  if (!(ret = (unsigned char *)malloc(EVP_MD_CTX_size(&ctx)))) return 0;
  EVP_DigestFinal(&ctx, ret, outlen);
  return ret;
}
