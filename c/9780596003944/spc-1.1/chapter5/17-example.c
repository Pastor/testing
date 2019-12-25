#include <openssl/evp.h>
#include <openssl/rand.h>

/* key must be of size EVP_MAX_KEY_LENGTH.
 * iv must be of size EVP_MAX_IV_LENGTH.
 */
EVP_CIPHER_CTX *sample_setup(unsigned char *key, unsigned char *iv) {
  EVP_CIPHER_CTX *ctx;

  /* This uses the OpenSSL PRNG .  See Recipe  11.9 */
  RAND_bytes(key, EVP_MAX_KEY_LENGTH);
  RAND_bytes(iv, EVP_MAX_IV_LENGTH);
  if (!(ctx = (EVP_CIPHER_CTX *)malloc(sizeof(EVP_CIPHER_CTX)))) return 0;
  EVP_CIPHER_CTX_init(ctx);
  EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), 0, key, iv);
  return ctx;
}
