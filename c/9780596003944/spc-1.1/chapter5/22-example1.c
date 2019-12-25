#include <stdlib.h>
#include <openssl/evp.h>

/* The integer pointed to by rb receives the number of bytes in the output.
 * Note that the malloced buffer can be realloced right before the return.
 */
char *encrypt_example(EVP_CIPHER_CTX *ctx, char *data, int inl, int *rb) {
  int  i, ol, tmp;
  char *ret;

  ol = 0;
  if (!(ret = (char *)malloc(i = inl + EVP_CIPHER_CTX_block_size(ctx)))) abort();
  if (i < inl) abort();
  for (i = 0;  i < inl / 100;  i++) {
    if (!EVP_EncryptUpdate(ctx, &ret[ol], &tmp, &data[ol], 100)) abort();
    ol += tmp;
  }
  if (inl % 100) {
    if (!EVP_EncryptUpdate(ctx, &ret[ol], &tmp, &data[ol], inl % 100)) abort();
    ol += tmp;
  }
  if (!EVP_EncryptFinal_ex(ctx, &ret[ol], &tmp)) abort();
  ol += tmp;
  if (rb) *rb = ol;
  return ret;
}
