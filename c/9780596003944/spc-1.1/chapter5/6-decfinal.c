int spc_cbc_decrypt_final(SPC_CBC_CTX *ctx, unsigned char *out, size_t *ol) {
  unsigned int i;
  unsigned char pad;

  if (ctx->ix != SPC_BLOCK_SZ) {
    if (ol) *ol = 0;
    /* If there was no input, and there's no padding, then everything is OK. */
    spc_memset(&(ctx->ks), 0, sizeof(SPC_KEY_SCHED));
    spc_memset(ctx, 0, sizeof(SPC_CBC_CTX));
    return (!ctx->ix && !ctx->pad);
  }
  if (!ctx->pad) {
    SPC_DO_DECRYPT(&(ctx->ks), ctx->ctbuf, out);
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
      ((int *)out)[i] ^= ((int *)ctx->iv)[i];
    if (ol) *ol = SPC_BLOCK_SZ;
    spc_memset(ctx, 0, sizeof(SPC_CBC_CTX));
    return 1;
  }
  SPC_DO_DECRYPT(&(ctx->ks), ctx->ctbuf, ctx->ctbuf);
  spc_memset(&(ctx->ks), 0, sizeof(SPC_KEY_SCHED));
  for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
    ((int *)ctx->ctbuf)[i] ^= ((int *)ctx->iv)[i];
  pad = ctx->ctbuf[SPC_BLOCK_SZ - 1];
  if (pad > SPC_BLOCK_SZ) {
    if (ol) *ol = 0;
    spc_memset(ctx, 0, sizeof(SPC_CBC_CTX));
    return 0;
  }
  for (i = 1;  i < pad;  i++) {
    if (ctx->ctbuf[SPC_BLOCK_SZ - 1 - i] != pad) {
      if (ol) *ol = 0;
      spc_memset(ctx, 0, sizeof(SPC_CBC_CTX));
      return 0;
    }
  }
  for (i = 0;  i < SPC_BLOCK_SZ - pad;  i++)
    *out++ = ctx->ctbuf[i];
  if (ol) *ol = SPC_BLOCK_SZ - pad;
  spc_memset(ctx, 0, sizeof(SPC_CBC_CTX));
  return 1;
}
