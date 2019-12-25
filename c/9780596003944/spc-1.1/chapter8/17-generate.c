#include <openssl/bn.h>

typedef struct {
  BIGNUM *n;
  BIGNUM *g; /* use a BIGNUM even though g is usually small. */
  BIGNUM *private_value;
  BIGNUM *public_value;
} DH_CTX;

/* This function assumes that all BIGNUMs are already allocated, and that n and g
 * have already been chosen and properly initialized.  After this function
 * completes successfully, use BN_bn2bin() on ctx->public_value to get a binary
 * representation you can send over a network.  See Recipe 7.4 for more info on
 * BN<->binary conversions.
 */
int DH_generate_keys(DH_CTX *ctx) {
  BN_CTX *tmp_ctx;

  if (!(tmp_ctx = BN_CTX_new())) return 0;
  if (!BN_rand_range(ctx->private_value, ctx->n)) {
    BN_CTX_free(tmp_ctx);
    return 0;
  }
  if (!BN_mod_exp(ctx->public_value, ctx->g, ctx->private_value, ctx->n, tmp_ctx)) {
    BN_CTX_free(tmp_ctx);
    return 0;
  }
  BN_CTX_free(tmp_ctx);
  return 1;
}
