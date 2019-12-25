int spc_cbc_encrypt_update(SPC_CBC_CTX *ctx, unsigned char *in, size_t il,
                           unsigned char *out, size_t *ol) {
  /* Keep a ptr to in, which we advance; we calculate ol by subtraction later. */
  int           i;
  unsigned char *start = out;

  /* If we have leftovers, but not enough to fill a block, XOR them into the right
   * places in the IV slot and return.  It's not much stuff, so one byte at a time
   * is fine.
   */
  if (il < SPC_BLOCK_SZ-ctx->ix) {
    while (il--) ctx->iv[ctx->ix++] ^= *in++;
    if (ol) *ol = 0;
    return 1;
  }

  /* If we did have leftovers, and we're here, fill up a block then output the
   * ciphertext.
   */
  if (ctx->ix) {
    while (ctx->ix < SPC_BLOCK_SZ) --il, ctx->iv[ctx->ix++] ^= *in++;
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->iv, ctx->iv);
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
      ((unsigned int *)out)[i] = ((unsigned int *)(ctx->iv))[i];
    out += SPC_BLOCK_SZ;
  }

  /* Operate on word-sized chunks, because it's easy to do so.  You might gain a
   * couple of cycles per loop by unrolling and getting rid of i if you know your
   * word size a priori.
   */
  while (il >= SPC_BLOCK_SZ) {
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
      ((unsigned int *)(ctx->iv))[i] ^= ((unsigned int *)in)[i];
    SPC_DO_ENCRYPT(&(ctx->ks), ctx->iv, ctx->iv);
    for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
      ((unsigned int *)out)[i] = ((unsigned int *)(ctx->iv))[i];
    out += SPC_BLOCK_SZ;
    in  += SPC_BLOCK_SZ;
    il  -= SPC_BLOCK_SZ;
  }

  /* Deal with leftovers... one byte at a time is fine. */
  for (i = 0;  i < il;  i++) ctx->iv[i] ^= in[i];
  ctx->ix = il;
  if (ol) *ol = out-start;
  return 1;
}
