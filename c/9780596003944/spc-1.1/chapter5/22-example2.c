#include <stdlib.h>
#include <openssl/evp.h>

char *decrypt_example(EVP_CIPHER_CTX *ctx, char *ct, int inl) {
  /* We're going to null-terminate the plaintext under the assumption that it's
   * non-null terminated ASCII text.  The null can otherwise be ignored if it
   * wasn't necessary, though the length of the result should be passed back in
   * such a case.
   */
  int  ol;
  char *pt;

  if (!(pt = (char *)malloc(i = inl + EVP_CIPHER_CTX_block_size(ctx) + 1))) abort();
  if (i < inl) abort();
  EVP_DecryptUpdate(ctx, pt, &ol, ct, inl);
  if (!ol) { /* There is no data to decrypt */
    free(pt);
    return 0;
  }
  pt[ol] = 0;
  return pt;
}
