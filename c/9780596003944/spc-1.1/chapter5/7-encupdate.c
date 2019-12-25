int spc_cfb_encrypt_update(SPC_CFB_CTX *ctx, unsigned char *in, size_t il,
                           unsigned char *out) {
  int i;

  if (ctx->ix) {
    while (ctx->ix) {
      if (!il--) return 1;
      ctx->nonce[ctx->ix] = *out++ = *in++ ^ ctx->nonce[ctx->ix++];
      ctx->ix %= SPC_BLOCK_SZ;
    }
  }
  if (!il) return 1;
  while (il >= SPC_BLOCK_SZ) {
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->nonce, ctx->nonce);
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++) {
      ((int *)ctx->nonce)[i] = ((int *)out)[i] = ((int *)in)[i] ^
                                                 ((int *)ctx->nonce)[i];

    }
    il  -= SPC_BLOCK_SZ;
    in  += SPC_BLOCK_SZ;
    out += SPC_BLOCK_SZ;
  }
  SPC_DO_ENCRYPT(&(ctx->ks), ctx->nonce, ctx->nonce);
  for (i = 0;  i <il;  i++)
    ctx->nonce[ctx->ix] = *out++ = *in++ ^ ctx->nonce[ctx->ix++];
  return 1;
}
