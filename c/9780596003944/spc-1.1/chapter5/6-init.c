void spc_cbc_encrypt_init(SPC_CBC_CTX *ctx, unsigned char *key, size_t kl,
                          unsigned char *iv) {
  SPC_ENCRYPT_INIT(&(ctx->ks), key, kl);
  spc_memset(key, 0, kl);
  memcpy(ctx->iv, iv, SPC_BLOCK_SZ);
  ctx->ix  = 0;
  ctx->pad = 1;
}

void spc_cbc_decrypt_init(SPC_CBC_CTX *ctx, unsigned char *key, size_t kl,
                          unsigned char *iv) {
  SPC_DECRYPT_INIT(&(ctx->ks), key, kl);
  spc_memset(key, 0, kl);
  memcpy(ctx->iv, iv, SPC_BLOCK_SZ);
  ctx->ix  = 0;
  ctx->pad = 1;
}
