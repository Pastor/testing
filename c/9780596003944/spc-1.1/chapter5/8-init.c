void spc_ofb_init(SPC_OFB_CTX *ctx, unsigned char *key, size_t kl,
                  unsigned char *nonce) {
  SPC_ENCRYPT_INIT(&(ctx->ks), key, kl);
  spc_memset(key,0, kl);
  memcpy(ctx->nonce, nonce, SPC_BLOCK_SZ);
  ctx->ix = 0;
}
