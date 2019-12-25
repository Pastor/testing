#include <openssl/bn.h>

typedef struct {
  BIGNUM        *n;
  unsigned long e; /* This number should generally be small. */
} RSA_PUBKEY;

typedef struct {
  BIGNUM *n;
  BIGNUM *d;  /* The actual private key. */

  /* These aren't necessary, but speed things up if used. If you do use them,
     you don't need to keep n or d around. */
  BIGNUM *p;
  BIGNUM *q;
  BIGNUM *dP, *dQ, *qInv;
} RSA_PRIVATE;

void spc_keypair_from_primes(BIGNUM *p, BIGNUM *q, unsigned long e,
                             RSA_PUBKEY *pubkey, RSA_PRIVATE *privkey)
{
  BN_CTX *x = BN_CTX_new();
  BIGNUM p_minus_1, q_minus_1, one, tmp, bn_e;

  pubkey->n  = privkey->n = BN_new();
  privkey->d = BN_new();
  pubkey->e  = e;
  privkey->p = p;
  privkey->q = q;

  BN_mul(pubkey->n, p, q, x);
  BN_init(&p_minus_1);
  BN_init(&q_minus_1);
  BN_init(&one);
  BN_init(&tmp);
  BN_init(&bn_e);
  BN_set_word(&bn_e, e);
  BN_one(&one);
  BN_sub(&p_minus_1, p, &one);
  BN_sub(&q_minus_1, q, &one);
  BN_mul(&tmp, &p_minus_1, &q_minus_1, x);
  BN_mod_inverse(privkey->d, &bn_e, &tmp, x);

  /* Compute extra values. */
  privkey->dP   = BN_new();
  privkey->dQ   = BN_new();
  privkey->qInv = BN_new();

  BN_mod(privkey->dP, privkey->d, &p_minus_1, x);
  BN_mod(privkey->dQ, privkey->d, &q_minus_1, x);
  BN_mod_inverse(privkey->qInv, q, p, x);
}
