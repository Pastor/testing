#include <openssl/evp.h>

EVP_CIPHER_CTX *blowfish_256_cbc_setup(char *key, char *iv) {
  EVP_CIPHER_CTX *ctx;

  if (!(ctx = (EVP_CIPHER_CTX *)malloc(sizeof(EVP_CIPHER_CTX)))) return 0;
  EVP_CIPHER_CTX_init(ctx);

  /* Uses 128-bit keys by default. We pass in NULLs for the parameters that we'll
   * fill in after properly setting the key length.
   */
  EVP_EncryptInit_ex(ctx, EVP_bf_cbc(), 0, 0, 0);
  EVP_CIPHER_CTX_set_key_length(ctx, 32);
  EVP_EncryptInit_ex(ctx, 0, 0, key, iv);
  return ctx;
}
