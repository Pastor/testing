#include <stdlib.h>

typedef struct {
  SPC_KEY_SCHED ks;
  int           ix;
  unsigned char iv[SPC_BLOCK_SZ];
  unsigned char c1[SPC_BLOCK_SZ]; /* L * u */
  unsigned char c2[SPC_BLOCK_SZ]; /* L / u */
} SPC_OMAC_CTX;

int spc_omac1_init(SPC_OMAC_CTX *ctx, unsigned char *key, int keylen) {
  int           condition, i;
  unsigned char L[SPC_BLOCK_SZ] = {0,};

  if (keylen != 16 && keylen != 24 && keylen != 32) return 0;

  SPC_ENCRYPT_INIT(&(ctx->ks), key, keylen);
  SPC_DO_ENCRYPT(&(ctx->ks), L, L);
  spc_memset(ctx->iv, 0, SPC_BLOCK_SZ);
  ctx->ix = 0;

  /* Compute L * u */
  condition = L[0] & 0x80;
  ctx->c1[0] = L[0] << 1;
  for (i = 1;  i < SPC_BLOCK_SZ; i++) {
    ctx->c1[i - 1] |= L[i] >> 7;
    ctx->c1[i]      = L[i] << 1;
  }
  if (condition) ctx->c1[SPC_BLOCK_SZ - 1] ^= 0x87;

  /* Compute L * u * u */
  condition  = ctx->c1[0] & 0x80;
  ctx->c2[0] = ctx->c1[0] << 1;
  for (i = 1;  i < SPC_BLOCK_SZ;  i++) {
    ctx->c2[i - 1] |= ctx->c1[i] >> 7;
    ctx->c2[i]      = ctx->c1[i] << 1;
  }
  if (condition) ctx->c2[SPC_BLOCK_SZ - 1] ^= 0x87;
  spc_memset(L, 0, SPC_BLOCK_SZ);
  return 1;
}

int spc_omac2_init(SPC_OMAC_CTX *ctx, unsigned char *key, int keylen) {
  int           condition, i;
  unsigned char L[SPC_BLOCK_SZ] = {0,};

  if (keylen != 16 && keylen != 24 && keylen != 32) return 0;

  SPC_ENCRYPT_INIT(&(ctx->ks), key, keylen);
  SPC_DO_ENCRYPT(&(ctx->ks), L, L);
  spc_memset(ctx->iv, 0, SPC_BLOCK_SZ);
  ctx->ix = 0;

  /* Compute L * u, storing it in c1 */
  condition  = L[0] >> 7;
  ctx->c1[0] = L[0] << 1;
  for (i = 1;  i < SPC_BLOCK_SZ;  i++) {
    ctx->c1[i - 1] |= L[i] >> 7;
    ctx->c1[i]      = L[i] << 1;
  }
  if (condition) ctx->c1[SPC_BLOCK_SZ - 1] ^= 0x87;

  /* Compute L * u ^ -1, storing it in c2 */
  condition = L[SPC_BLOCK_SZ - 1] & 0x01;
  i = SPC_BLOCK_SZ;
  while (--i) ctx->c2[i] = (L[i] >> 1) | (L[i - 1] << 7);
  ctx->c2[0] = L[0] >> 1;
  L[0] >>= 1;
  if (condition) {
    ctx->c2[0]                ^= 0x80;
    ctx->c2[SPC_BLOCK_SZ - 1] ^= 0x43;
  }
  spc_memset(L, 0, SPC_BLOCK_SZ);
  return 1;
}

void spc_omac_update(SPC_OMAC_CTX *ctx, unsigned char *in, size_t il) {
  int i;

  if (il < SPC_BLOCK_SZ - ctx->ix) {
    while (il--) ctx->iv[ctx->ix++] ^= *in++;
    return;
  }
  if (ctx->ix) {
    while (ctx->ix < SPC_BLOCK_SZ) --il, ctx->iv[ctx->ix++] ^= *in;
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->iv, ctx->iv);
  }
  while (il > SPC_BLOCK_SZ) {
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
      ((unsigned int *)(ctx->iv))[i] ^= ((unsigned int *)in)[i];
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->iv, ctx->iv);
    in += SPC_BLOCK_SZ;
    il -= SPC_BLOCK_SZ;
  }
  for (i = 0;  i < il;  i++) ctx->iv[i] ^= in[i];
  ctx->ix = il;
}

int spc_omac_final(SPC_OMAC_CTX *ctx, unsigned char *out) {
  int i;

  if (ctx->ix != SPC_BLOCK_SZ) {
    ctx->iv[ctx->ix] ^= 0x80;
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
      ((int *)ctx->iv)[i] ^= ((int *)ctx->c2)[i];
  } else {
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
      ((int *)ctx->iv)[i] ^= ((int *)ctx->c1)[i];
  }
  SPC_DO_ENCRYPT(&(ctx->ks), ctx->iv, out);
  return 1;
}
