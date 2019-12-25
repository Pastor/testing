#include <stdlib.h>
#include <string.h>

unsigned char *spc_ctr_encrypt(unsigned char *key, size_t kl, unsigned char *nonce,
                               unsigned char *in, size_t il) {
  SPC_CTR_CTX   ctx;
  unsigned char *out;

  if (!(out = (unsigned char *)malloc(il))) return 0;
  spc_ctr_init(&ctx, key, kl, nonce);
  spc_ctr_update(&ctx, in, il, out);
  spc_ctr_final(&ctx);
  return out;
}

#define spc_ctr_decrypt spc_ctr_encrypt
