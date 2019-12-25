BIGNUM *DH_compute_secret(DH_CTX *ctx, BIGNUM *received) {
  BIGNUM *secret;
  BN_CTX *tmp_ctx;

  if (!(secret = BN_new())) return 0;
  if (!(tmp_ctx = BN_CTX_new())) {
    BN_free(secret);
    return 0;
  }
  if (!BN_mod_exp(secret, received, ctx->private_value, ctx->n, tmp_ctx)) {
    BN_CTX_free(tmp_ctx);
    BN_free(secret);
    return 0;
  }
  BN_CTX_free(tmp_ctx);
  return secret;
}
