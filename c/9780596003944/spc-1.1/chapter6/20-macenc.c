#include <stdlib.h>
#include <string.h>

#define NONCE_LEN  16
#define CTR_LEN    16
#define MAC_OUT_SZ 20

unsigned char *spc_MAC_encrypt(unsigned char *in, size_t len, unsigned char *key,
                                 int keylen, unsigned char *nonce) {
  /* We're using a 128-bit nonce and a 128-bit counter, packed into one variable */
  int           i;
  size_t        blks;
  SPC_HMAC_CTX  ctx;
  unsigned char ctr[NONCE_LEN + CTR_LEN];
  unsigned char keystream[MAC_OUT_SZ];
  unsigned char *out;

  if (!(out = (unsigned char *)malloc(len))) abort();
  SPC_HMAC_Init(&ctx, key, keylen);
  memcpy(ctr, nonce, NONCE_LEN);
  memset(ctr + NONCE_LEN, 0, CTR_LEN);
  blks = len / MAC_OUT_SZ;
  while (blks--) {
    SPC_HMAC_Reset(&ctx);
    SPC_HMAC_Update(&ctx, ctr, sizeof(ctr));
    SPC_HMAC_Final(out, &ctx);
    i = NONCE_LEN + CTR_LEN;
    /* Increment the counter. */
    while (i-- != NONCE_LEN)
      if (++ctr[i]) break;
    for (i = 0;  i < MAC_OUT_SZ;  i++) *out++ = *in++ ^ keystream[i];
  }
  if (len % MAC_OUT_SZ) {
    SPC_HMAC_Reset(&ctx);
    SPC_HMAC_Update(&ctx, ctr, sizeof(ctr));
    SPC_HMAC_Final(out, &ctx);
    for (i = 0;  i < len % MAC_OUT_SZ;  i++) *out++ = *in++ ^ keystream[i];
  }
  return out;
}
