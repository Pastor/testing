static inline void ctr_increment(unsigned char *ctr) {
  unsigned char *x = ctr + SPC_CTR_BYTES;

  while (x-- != ctr) if (++(*x)) return;
}

int spc_ctr_update(SPC_CTR_CTX *ctx, unsigned char *in, size_t il,
                   unsigned char *out) {
  int i;

  if (ctx->ix) {
    while (ctx->ix) {
      if (!il--) return 1;
      *out++ = *in++ ^ ctx->ksm[ctx->ix++];
      ctx->ix %= SPC_BLOCK_SZ;
    }
  }
  if (!il) return 1;
  while (il >= SPC_BLOCK_SZ) {
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->ctr, out);
    ctr_increment(ctx->ctr);
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
      ((int *)out)[i] ^= ((int *)in)[i];
    il  -= SPC_BLOCK_SZ;
    in  += SPC_BLOCK_SZ;
    out += SPC_BLOCK_SZ;
  }
  SPC_DO_ENCRYPT(&(ctx->ks), ctx->ctr, ctx->ksm);
  ctr_increment(ctx->ctr);
  for (i = 0;  i < il;  i++)
    *out++ = *in++ ^ ctx->ksm[ctx->ix++];
  return 1;
}
