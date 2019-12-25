#define SPC_CTR_BYTES 6

void spc_ctr_init(SPC_CTR_CTX *ctx, unsigned char *key, size_t kl,
                  unsigned char *nonce) {
  SPC_ENCRYPT_INIT(&(ctx->ks), key, kl);
  spc_memset(key, 0, kl);
  memcpy(ctx->ctr, nonce, SPC_BLOCK_SZ - SPC_CTR_BYTES);
  spc_memset(ctx->ctr + SPC_BLOCK_SZ - SPC_CTR_BYTES, 0, SPC_CTR_BYTES);
  ctx->ix = 0;
}
