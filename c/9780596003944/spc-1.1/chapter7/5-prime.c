#include <stdlib.h>
#include <openssl/bn.h>

#define NUMBER_ITERS    5
#define NUMBER_PRIMES   100

static unsigned long primes[NUMBER_PRIMES] = {
  2,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37, 41,  43,  47,  53,
  59,  61,  67,  71,  73,  79,  83,  89,  97,  101, 103, 107, 109, 113, 127, 131,
  137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223,
  227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311,
  313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
  419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503,
  509, 521, 523, 541
};

static int is_obviously_not_prime(BIGNUM *p);
static int passes_rabin_miller_once(BIGNUM *p);
static unsigned int calc_b_and_m(BIGNUM *p, BIGNUM *m);

int spc_is_probably_prime(BIGNUM *p) {
  int i;

  if (is_obviously_not_prime(p)) return 0;
  for (i = 0;  i < NUMBER_ITERS;  i++)
    if (!passes_rabin_miller_once(p))
      return 0;
  return 1;
}

BIGNUM *spc_generate_prime(int nbits) {
  BIGNUM        *p = BN_new();
  unsigned char binary_rep[nbits / 8];

  /* This code assumes we'll only ever want to generate primes with the number of
   * bits a multiple of eight!
   */
  if (nbits % 8 || !p) abort();

  for (;;) {
    spc_rand(binary_rep, nbits / 8);

    /* Set the two most significant and the least significant bits to 1. */
    binary_rep[0] |= 0xc0;
    binary_rep[nbits / 8 - 1] |= 1;

    /* Convert this number to its BIGNUM representation */
    if (!BN_bin2bn(binary_rep, nbits / 8, p)) abort();

    /* If you're going to test for suitability as a Diffie-Hellman prime, do so
     * before calling spc_is_probably_prime(p).
     */
    if (spc_is_probably_prime(p)) return p;
  }
}

/* Try simple division with all our small primes.  This is, for each prime, if it
 * evenly divides p, return 0.  Note that this obviously doesn't work if we're
 * checking a prime number that's in the list!
 */
static int is_obviously_not_prime(BIGNUM *p) {
  int i;

  for (i = 0;  i < NUMBER_PRIMES;  i++)
    if (!BN_mod_word(p, primes[i])) return 1;
  return 0;
}

static int passes_rabin_miller_once(BIGNUM *p) {
  BIGNUM       a, m, z, tmp;
  BN_CTX       *ctx;
  unsigned int b, i;

  /* Initialize a, m, z and tmp properly. */
  BN_init(&a);
  BN_init(&m);
  BN_init(&z);
  BN_init(&tmp);

  ctx = BN_CTX_new();
  b = calc_b_and_m(p, &m);

  /* a is a random number less than p: */
  if (!BN_rand_range(&a, p)) abort();

  /* z = a^m mod p. */
  if (!BN_mod_exp(&z, &a, &m, p, ctx)) abort();

  /* if z = 1 at the start, pass. */
  if (BN_is_one(&z)) return 1;

  for (i = 0;  i < b;  i++) {
    if (BN_is_one(&z)) return 0;

    /* if z = p-1, pass! */
    BN_copy(&tmp, &z);
    if (!BN_add_word(&tmp, 1)) abort();
    if (!BN_cmp(&tmp, p)) return 1;

    /* z = z^2 mod p */
    BN_mod_sqr(&tmp, &z, p, ctx);
    BN_copy(&z, &tmp);
  }

  /* if z = p-1, pass! */
  BN_copy(&tmp, &z);
  if (!BN_add_word(&tmp, 1)) abort();
  if (!BN_cmp(&tmp, p)) return 1;

  /* Fail! */
  return 0;
}

/* b = How many times does 2 divide p - 1?   This gets returned.
 * m is (p-1)/(2^b).
 */
static unsigned int calc_b_and_m(BIGNUM *p, BIGNUM *x) {
  unsigned int b;

  if (!BN_copy(x, p)) abort();
  if (!BN_sub_word(x, 1))  abort();

  for (b = 0;  !BN_is_odd(x);  b++)
    BN_div_word(x, 2);
  return b;
}
