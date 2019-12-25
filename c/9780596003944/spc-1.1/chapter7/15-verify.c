#include <openssl/dsa.h>
#include <openssl/sha.h>
#include <openssl/objects.h>

int spc_DSA_verify(unsigned char *msg, int msglen, unsigned char *sig, int siglen,
                   DSA *dsa) {
  unsigned char hash[20];

  if (!SHA1(msg, msglen, hash)) return 0;
  return DSA_verify(NID_sha1, hash, 20, sig, siglen, dsa);
}
