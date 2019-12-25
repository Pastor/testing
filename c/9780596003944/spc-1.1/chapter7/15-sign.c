#include <openssl/dsa.h>
#include <openssl/sha.h>
#include <openssl/objects.h>

int spc_DSA_sign(unsigned char *msg, int msglen, unsigned char *sig, DSA *dsa) {
  unsigned int  ignored;
  unsigned char hash[20];

  if (!SHA1(msg, msglen, hash)) return 0;
  return DSA_sign(NID_sha1, hash, 20, sig, &ignored, dsa);
}
