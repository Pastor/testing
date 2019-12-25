int spc_cfb_decrypt_update(SPC_CFB_CTX *ctx, unsigned char *in, size_t il,
                           unsigned char *out) {
  int  i, x;
  char c;

  if (ctx->ix) {
    while (ctx->ix) {
      if (!il--) return 1;
      c = *in;
      *out++ = *in++ ^ ctx->nonce[ctx->ix];
      ctx->nonce[ctx->ix++] = c;
      ctx->ix %= SPC_BLOCK_SZ;
    }
  }
  if (!il) return 1;
  while (il >= SPC_BLOCK_SZ) {
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->nonce, ctx->nonce);
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++) {
      x = ((int *)in)[i];
      ((int *)out)[i] = x ^ ((int *)ctx->nonce)[i];
      ((int *)ctx->nonce)[i] = x;
    }
    il  -= SPC_BLOCK_SZ;
    in  += SPC_BLOCK_SZ;
    out += SPC_BLOCK_SZ;
  }
  SPC_DO_ENCRYPT(&(ctx->ks), ctx->nonce, ctx->nonce);
  for (i = 0;  i < il;  i++) {
    c = *in;
    *out++ = *in++ ^ ctx->nonce[ctx->ix];
    ctx->nonce[ctx->ix++] = c;
  }
  return 1;
}
