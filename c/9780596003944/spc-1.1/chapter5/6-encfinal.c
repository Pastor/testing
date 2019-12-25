int spc_cbc_encrypt_final(SPC_CBC_CTX *ctx, unsigned char *out, size_t *ol) {
  int           ret;
  unsigned char pad;

  if (ctx->pad) {
    pad = SPC_BLOCK_SZ - ctx->ix;
    while (ctx->ix < SPC_BLOCK_SZ) ctx->iv[ctx->ix++] ^= pad;
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->iv, out);
    spc_memset(ctx, 0, sizeof(SPC_CBC_CTX));
    if(ol) *ol = SPC_BLOCK_SZ;
    return 1;
  }
  if(ol) *ol = 0;
  ret = !(ctx->ix);
  spc_memset(ctx, 0, sizeof(SPC_CBC_CTX));
  return ret;
}
