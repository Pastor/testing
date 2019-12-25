#include <stdlib.h>
#include <string.h>

unsigned char *spc_cfb_encrypt(unsigned char *key, size_t kl, unsigned char *nonce,
                               unsigned char *in, size_t il) {
  SPC_CFB_CTX   ctx;
  unsigned char *out;

  if (!(out = (unsigned char *)malloc(il))) return 0;
  spc_cfb_init(&ctx, key, kl, nonce);
  spc_cfb_encrypt_update(&ctx, in, il, out);
  spc_cfb_final(&ctx);
  return out;
}

unsigned char *spc_cfb_decrypt(unsigned char *key, size_t kl, unsigned char *nonce,
                               unsigned char *in, size_t il) {
  SPC_CFB_CTX   ctx;
  unsigned char *out;

  if (!(out = (unsigned char *)malloc(il))) return 0;
  spc_cfb_init(&ctx, key, kl, nonce);
  spc_cfb_decrypt_update(&ctx, in, il, out);
  spc_cfb_final(&ctx);
  return out;
}
