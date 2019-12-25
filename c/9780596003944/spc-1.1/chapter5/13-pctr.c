#include <stdlib.h>
#include <string.h>

typedef struct {
  SPC_KEY_SCHED ks;
  size_t        len;
  unsigned char ctr_odd[SPC_BLOCK_SZ];
  unsigned char ctr_even[SPC_BLOCK_SZ];
  unsigned char *inptr_odd;
  unsigned char *inptr_even;
  unsigned char *outptr_odd;
  unsigned char *outptr_even;
} SPC_CTR2_CTX;

static void pctr_increment(unsigned char *ctr) {
  unsigned char *x = ctr + SPC_CTR_BYTES;

  while (x-- != ctr) if (++(*x)) return;
}

void spc_pctr_setup(SPC_CTR2_CTX *ctx, unsigned char *key, size_t kl,
                    unsigned char *nonce, unsigned char *in, size_t len,
                    unsigned char *out) {
  SPC_ENCRYPT_INIT(&(ctx->ks), key, kl);
  spc_memset(key,0, kl);
  memcpy(ctx->ctr_odd, nonce, SPC_BLOCK_SZ - SPC_CTR_BYTES);
  spc_memset(ctx->ctr_odd + SPC_BLOCK_SZ - SPC_CTR_BYTES, 0, SPC_CTR_BYTES);
  memcpy(ctx->ctr_even, nonce, SPC_BLOCK_SZ - SPC_CTR_BYTES);
  spc_memset(ctx->ctr_even + SPC_BLOCK_SZ - SPC_CTR_BYTES, 0, SPC_CTR_BYTES);
  pctr_increment(ctx->ctr_even);
  ctx->inptr_odd   = in;
  ctx->inptr_even  = in + SPC_BLOCK_SZ;
  ctx->outptr_odd  = out;
  ctx->outptr_even = out + SPC_BLOCK_SZ;
  ctx->len         = len;
}

void spc_pctr_do_odd(SPC_CTR2_CTX *ctx) {
  size_t        i, j;
  unsigned char final[SPC_BLOCK_SZ];

  for (i = 0;  i + SPC_BLOCK_SZ < ctx->len;  i += 2 * SPC_BLOCK_SZ) {
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->ctr_odd, ctx->outptr_odd);
    pctr_increment(ctx->ctr_odd);
    pctr_increment(ctx->ctr_odd);
    for (j = 0;  j < SPC_BLOCK_SZ / sizeof(int);  j++)
      ((int *)ctx->outptr_odd)[j] ^= ((int *)ctx->inptr_odd)[j];
    ctx->outptr_odd += SPC_BLOCK_SZ * 2;
    ctx->inptr_odd  += SPC_BLOCK_SZ * 2;
  }
  if (i < ctx->len) {
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->ctr_odd, final);
    for (j = 0;  j < ctx->len - i;  j++)
      ctx->outptr_odd[j] = final[j] ^ ctx->inptr_odd[j];
  }
}

void spc_pctr_do_even(SPC_CTR2_CTX *ctx) {
  size_t        i, j;
  unsigned char final[SPC_BLOCK_SZ];

  for (i = SPC_BLOCK_SZ; i + SPC_BLOCK_SZ < ctx->len;  i += 2 * SPC_BLOCK_SZ) {
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->ctr_even, ctx->outptr_even);
    pctr_increment(ctx->ctr_even);
    pctr_increment(ctx->ctr_even);
    for (j = 0;  j < SPC_BLOCK_SZ / sizeof(int);  j++)
      ((int *)ctx->outptr_even)[j] ^= ((int *)ctx->inptr_even)[j];
    ctx->outptr_even += SPC_BLOCK_SZ * 2;
    ctx->inptr_even  += SPC_BLOCK_SZ * 2;
  }
  if (i < ctx->len) {
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->ctr_even, final);
    for (j = 0;  j < ctx->len - i;  j++)
      ctx->outptr_even[j] = final[j] ^ ctx->inptr_even[j];
  }
}

int spc_pctr_final(SPC_CTR2_CTX *ctx) {
  spc_memset(&ctx, 0, sizeof(SPC_CTR2_CTX));
  return 1;
}
