#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>

unsigned char *spc_create_nonced_digest(EVP_MD *type, unsigned char *in,
                                        unsigned long n, unsigned int *outlen) {
  int           bsz, dlen;
  EVP_MD_CTX    ctx;
  unsigned char *pad, *ret;

  EVP_DigestInit(&ctx, type);
  dlen = EVP_MD_CTX_size(&ctx);
  if (!(ret = (unsigned char *)malloc(dlen * 2))) return 0;
  RAND_bytes(ret, dlen);
  EVP_DigestUpdate(&ctx, ret, dlen);

  bsz = EVP_MD_CTX_block_size(&ctx);
  if (!(pad = (unsigned char *)malloc(bsz - dlen))) {
    free(ret);
    return 0;
  }
  memset(pad, 0, bsz - dlen);
  EVP_DigestUpdate(&ctx, pad, bsz - dlen);
  EVP_DigestUpdate(&ctx, in, n);
  EVP_DigestUpdate(&ctx, ret, dlen);
  EVP_DigestUpdate(&ctx, pad, bsz - dlen);
  free(pad);
  EVP_DigestFinal(&ctx, ret + dlen, outlen);
  *outlen *= 2;
  return ret;
}

int spc_verify_nonced_digest(EVP_MD *type, unsigned char *in, unsigned long n,
                            unsigned char *toverify) {
  int           dlen, outlen, bsz, i;
  EVP_MD_CTX    ctx;
  unsigned char *pad, *vfy;

  EVP_DigestInit(&ctx, type);
  bsz  = EVP_MD_CTX_block_size(&ctx);
  dlen = EVP_MD_CTX_size(&ctx);
  EVP_DigestUpdate(&ctx, toverify, dlen);

  if (!(pad = (unsigned char *)malloc(bsz - dlen))) return 0;
  memset(pad, 0, bsz - dlen);
  EVP_DigestUpdate(&ctx, pad, bsz - dlen);
  EVP_DigestUpdate(&ctx, in, n);
  EVP_DigestUpdate(&ctx, toverify, dlen);
  EVP_DigestUpdate(&ctx, pad, bsz - dlen);
  free(pad);

  if (!(vfy = (unsigned char *)malloc(dlen))) return 0;
  EVP_DigestFinal(&ctx, vfy, &outlen);
  in += dlen;
  for (i = 0;  i < dlen;  i++)
    if (vfy[i] != toverify[i + dlen]) {
      free(vfy);
      return 0;
    }
  free(vfy);
  return 1;
}
