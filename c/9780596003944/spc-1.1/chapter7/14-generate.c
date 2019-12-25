#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/objects.h>
#include <openssl/rand.h>
#include <string.h>

#define MIN(x,y) ((x) > (y) ? (y) : (x))

unsigned char *generate_and_package_128_bit_secret(RSA *recip_pub_key,
                      RSA *signers_key, unsigned char *sec, unsigned int *olen) {
  unsigned char *tmp = 0, *to_encrypt = 0, *sig = 0, *out = 0, *p, *ptr;
  unsigned int  len, ignored, b_per_ct;
  int           bytes_remaining; /* MUST NOT BE UNSIGNED. */
  unsigned char hash[20];

  /* Generate the secret. */
  if (!RAND_bytes(sec, 16)) return 0;

  /* Now we need to sign the public key and the secret both.
   * Copy the secret into tmp, then the public key and the exponent.
   */
  len = 16 + RSA_size(recip_pub_key) + BN_num_bytes(recip_pub_key->e);
  if (!(tmp = (unsigned char *)malloc(len))) return 0;
  memcpy(tmp, sec, 16);
  if (!BN_bn2bin(recip_pub_key->n, tmp + 16)) goto err;
  if (!BN_bn2bin(recip_pub_key->e, tmp + 16 + RSA_size(recip_pub_key))) goto err;

  /* Now sign tmp (the hash of it), again mallocing space for the signature. */
  if (!(sig = (unsigned char *)malloc(BN_num_bytes(signers_key->n)))) goto err;
  if (!SHA1(tmp, len, hash)) goto err;
  if (!RSA_sign(NID_sha1, hash, 20, sig, &ignored, signers_key)) goto err;

  /* How many bytes we can encrypt each time, limited by the modulus size
   * and the padding requirements.
   */
  b_per_ct = RSA_size(recip_pub_key) - (2 * 20 + 2);

  if (!(to_encrypt = (unsigned char *)malloc(16 + RSA_size(signers_key))))
    goto err;

  /* The calculation before the mul is the number of encryptions we're
   * going to make.  After the mul is the output length of each
   * encryption.
   */
  *olen = ((16 + RSA_size(signers_key) + b_per_ct - 1) / b_per_ct) *
          RSA_size(recip_pub_key);
  if (!(out = (unsigned char *)malloc(*olen))) goto err;

  /* Copy the data to encrypt into a single buffer. */
  ptr = to_encrypt;
  bytes_remaining = 16 + RSA_size(signers_key);
  memcpy(to_encrypt, sec, 16);
  memcpy(to_encrypt + 16, sig, RSA_size(signers_key));
  p = out;

  while (bytes_remaining > 0) {
    /* encrypt b_per_ct bytes up until the last loop, where it may be fewer. */
    if (!RSA_public_encrypt(MIN(bytes_remaining,b_per_ct), ptr, p,
                           recip_pub_key, RSA_PKCS1_OAEP_PADDING)) {
        free(out);
        out = 0;
        goto err;
    }
    bytes_remaining -= b_per_ct;
    ptr += b_per_ct;
    /* Remember, output is larger than the input. */
    p += RSA_size(recip_pub_key);
  }

err:
  if (sig) free(sig);
  if (tmp) free(tmp);
  if (to_encrypt) free(to_encrypt);
  return out;
}
