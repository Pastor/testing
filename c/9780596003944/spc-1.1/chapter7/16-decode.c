#include <openssl/rsa.h>

/* Note that the pointer to the buffer gets copied in. Therefore, when
 * d2i_- changes its value, those changes aren't reflected in the caller's copy
 * of the pointer.
 */
RSA *DER_decode_RSA_public(unsigned char *buf, long len) {
  return d2i_RSAPublicKey(0, &buf, len);
}
