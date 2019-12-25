#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/objects.h>

int spc_sign(unsigned char *msg, unsigned int mlen, unsigned char *out,
             unsigned int *outlen, RSA *r) {
  unsigned char hash[20];

  if (!SHA1(msg, mlen, hash)) return 0;
  return RSA_sign(NID_sha1, hash, 20, out, outlen, r);
}
