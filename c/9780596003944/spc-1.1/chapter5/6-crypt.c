#include <stdlib.h>
#include <string.h>

unsigned char *spc_cbc_encrypt(unsigned char *key, size_t kl, unsigned char *iv,
                               unsigned char *in, size_t il, size_t *ol) {
  SPC_CBC_CTX   ctx;
  size_t        tmp;
  unsigned char *result;

  if (il + SPC_BLOCK_SZ < SPC_BLOCK_SZ) return 0;
  if (!(result = (unsigned char *)malloc(((il / SPC_BLOCK_SZ) * SPC_BLOCK_SZ) +
                                         SPC_BLOCK_SZ))) return 0;

  spc_cbc_encrypt_init(&ctx, key, kl, iv);
  spc_cbc_encrypt_update(&ctx, in, il, result, &tmp);
  spc_cbc_encrypt_final(&ctx, result+tmp, ol);
  *ol += tmp;
  return result;
}

unsigned char *spc_cbc_decrypt(unsigned char *key, size_t kl, unsigned char *iv,
                               unsigned char *in, size_t il, size_t *ol) {
  int           success;
  size_t        tmp;
  SPC_CBC_CTX   ctx;
  unsigned char *result;

  if (!(result = (unsigned char *)malloc(il))) return 0;
  spc_cbc_decrypt_init(&ctx, key, kl, iv);
  spc_cbc_decrypt_update(&ctx, in, il, result, &tmp);
  if (!(success = spc_cbc_decrypt_final(&ctx, result + tmp, ol))) {
    *ol = 0;
    spc_memset(result, 0, il);
    free(result);
    return 0;
  }
  *ol += tmp;
  result = (unsigned char *)realloc(result, *ol);
  return result;
}
