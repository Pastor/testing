#include <stdlib.h>
#include <string.h>

unsigned char *spc_ofb_encrypt(unsigned char *key, size_t kl, unsigned char *nonce,
                               unsigned char *in, size_t il) {
  SPC_OFB_CTX   ctx;
  unsigned char *out;

  if (!(out = (unsigned char *)malloc(il))) return 0;
  spc_ofb_init(&ctx, key, kl, nonce);
  spc_ofb_update(&ctx, in, il, out);
  spc_ofb_final(&ctx);
  return out;
}

#define spc_ofb_decrypt spc_ofb_encrypt
