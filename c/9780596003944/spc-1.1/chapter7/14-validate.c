#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/objects.h>
#include <openssl/rand.h>
#include <string.h>

#define MIN(x,y) ((x) > (y) ? (y) : (x))

/* recip_key must contain both the public and private key. */
int validate_and_retreive_secret(RSA *recip_key, RSA *signers_pub_key,
                                 unsigned char *encr, unsigned int inlen,
                                 unsigned char *secret) {
  int           result = 0;
  BN_CTX        *tctx;
  unsigned int  ctlen, stlen, i, l;
  unsigned char *decrypt, *signedtext, *p, hash[20];

  if (inlen % RSA_size(recip_key)) return 0;
  if (!(p = decrypt = (unsigned char *)malloc(inlen))) return 0;
  if (!(tctx = BN_CTX_new())) {
    free(decrypt);
    return 0;
  }
  RSA_blinding_on(recip_key, tctx);
  for (ctlen = i = 0;  i < inlen / RSA_size(recip_key);  i++) {
    if (!(l = RSA_private_decrypt(RSA_size(recip_key), encr, p, recip_key,
                                  RSA_PKCS1_OAEP_PADDING))) goto err;
    encr += RSA_size(recip_key);
    p += l;
    ctlen += l;
  }
  if (ctlen != 16 + RSA_size(signers_pub_key)) goto err;
  stlen = 16 + BN_num_bytes(recip_key->n) + BN_num_bytes(recip_key->e);
  if (!(signedtext = (unsigned char *)malloc(stlen))) goto err;
  memcpy(signedtext, decrypt, 16);
  if (!BN_bn2bin(recip_key->n, signedtext + 16)) goto err;
  if (!BN_bn2bin(recip_key->e, signedtext + 16 + RSA_size(recip_key))) goto err;
  if (!SHA1(signedtext, stlen, hash)) goto err;
  if (!RSA_verify(NID_sha1, hash, 20, decrypt + 16, RSA_size(signers_pub_key),
                  signers_pub_key)) goto err;
  memcpy(secret, decrypt, 16);
  result = 1;

err:
  RSA_blinding_off(recip_key);
  BN_CTX_free(tctx);
  free(decrypt);
  if (signedtext) free(signedtext);
  return result;
}
