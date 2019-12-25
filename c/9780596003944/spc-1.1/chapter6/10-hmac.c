#include <stdlib.h>
#include <string.h>

typedef struct {
  DGST_CTX      mdctx;
  unsigned char inner[DGST_BLK_SZ];
  unsigned char outer[DGST_BLK_SZ];
} SPC_HMAC_CTX;

void SPC_HMAC_Init(SPC_HMAC_CTX *ctx, unsigned char *key, size_t klen) {
  int           i;
  unsigned char dk[DGST_OUT_SZ];

  DGST_Init(&(ctx->mdctx));
  memset(ctx->inner, 0x36, DGST_BLK_SZ);
  memset(ctx->outer, 0x5c, DGST_BLK_SZ);

  if (klen <= DGST_BLK_SZ) {
    for (i = 0;  i < klen;  i++) {
      ctx->inner[i] ^= key[i];
      ctx->outer[i] ^= key[i];
    }
  } else {
    DGST_Update(&(ctx->mdctx), key, klen);
    DGST_Final(dk, &(ctx->mdctx));
    DGST_Reset(&(ctx->mdctx));
    for (i = 0;  i < DGST_OUT_SZ;  i++) {
      ctx->inner[i] ^= dk[i];
      ctx->outer[i] ^= dk[i];
    }
  }
  DGST_Update(&(ctx->mdctx), ctx->inner, DGST_BLK_SZ);
}

void SPC_HMAC_Reset(SPC_HMAC_CTX *ctx) {
  DGST_Reset(&(ctx->mdctx));
  DGST_Update(&(ctx->mdctx), ctx->inner, DGST_BLK_SZ);
}

void SPC_HMAC_Update(SPC_HMAC_CTX *ctx, unsigned char *m, size_t l) {
  DGST_Update(&(ctx->mdctx), m, l);
}

void SPC_HMAC_Final(unsigned char *tag, SPC_HMAC_CTX *ctx) {
  unsigned char is[DGST_OUT_SZ];

  DGST_Final(is, &(ctx->mdctx));
  DGST_Reset(&(ctx->mdctx));
  DGST_Update(&(ctx->mdctx), ctx->outer, DGST_BLK_SZ);
  DGST_Update(&(ctx->mdctx), is, DGST_OUT_SZ);
  DGST_Final(tag, &(ctx->mdctx));
}

void SPC_HMAC_Cleanup(SPC_HMAC_CTX *ctx) {
  volatile char *p = ctx->inner;
  volatile char *q = ctx->outer;
  int i;

  for (i = 0;  i < DGST_BLK_SZ;  i++) *p++ = *q++ = 0;
}
