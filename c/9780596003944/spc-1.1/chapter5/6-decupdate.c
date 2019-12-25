int spc_cbc_decrypt_update(SPC_CBC_CTX *ctx, unsigned char *in, size_t il,
                           unsigned char *out, size_t *ol) {
  int           i;
  unsigned char *next_iv, *start = out;

  /* If there's not enough stuff to fit in ctbuf, dump it in there and return */
  if (il < SPC_BLOCK_SZ - ctx->ix) {
    while (il--) ctx->ctbuf[ctx->ix++] = *in++;
    if (ol) *ol = 0;
    return 1;
  }

  /* If there's stuff in ctbuf, fill it. */
  if (ctx->ix % SPC_BLOCK_SZ) {
    while (ctx->ix < SPC_BLOCK_SZ) {
      ctx->ctbuf[ctx->ix++] = *in++;
      --il;
    }
  }
  if (!il) {
    if (ol) *ol = 0;
    return 1;
  }

  /* If we get here, and the ctbuf is full, it can't be padding.  Spill it. */
  if (ctx->ix) {
    SPC_DO_DECRYPT(&(ctx->ks), ctx->ctbuf, out);
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++) {
      ((int *)out)[i]    ^= ((int *)ctx->iv)[i];
      ((int *)ctx->iv)[i] = ((int *)ctx->ctbuf)[i];
    }
    out += SPC_BLOCK_SZ;
  }
  if (il > SPC_BLOCK_SZ) {
    SPC_DO_DECRYPT(&(ctx->ks), in, out);
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int); i++)
      ((int *)out)[i] ^= ((int *)ctx->iv)[i];
    next_iv = in;
    out  += SPC_BLOCK_SZ;
    in   += SPC_BLOCK_SZ;
    il   -= SPC_BLOCK_SZ;
  } else next_iv = ctx->iv;
  while (il > SPC_BLOCK_SZ) {
    SPC_DO_DECRYPT(&(ctx->ks), in, out);
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
      ((int *)out)[i] ^= ((int *)next_iv)[i];
    next_iv = in;
    out += SPC_BLOCK_SZ;
    in  += SPC_BLOCK_SZ;
    il  -= SPC_BLOCK_SZ;
  }

  /* Store the IV. */
  for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
    ((int *)ctx->iv)[i] = ((int *)next_iv)[i];
  ctx->ix = 0;
  while (il--) ctx->ctbuf[ctx->ix++] = *in++;
  if (ol) *ol = out - start;
  return 1;
}
