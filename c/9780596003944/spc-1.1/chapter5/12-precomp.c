int spc_ctr_keystream(SPC_CTR_CTX *ctx, size_t il, unsigned char *out) {
  int i;

  if (ctx->ix) {
    while (ctx->ix) {
      if (!il--) return 1;
      *out++ = ctx->ksm[ctx->ix++];
      ctx->ix %= SPC_BLOCK_SZ;
    }
  }
  if (!il) return 1;
  while (il >= SPC_BLOCK_SZ) {
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->ctr, out);
    ctr_increment(ctx->ctr);
    il  -= SPC_BLOCK_SZ;
    out += SPC_BLOCK_SZ;
  }
  SPC_DO_ENCRYPT(&(ctx->ks), ctx->ctr, ctx->ksm);
  ctr_increment(ctx->ctr);
  for (i = 0;  i <il;  i++) *out++ = ctx->ksm[ctx->ix++];
  return 1;
}
