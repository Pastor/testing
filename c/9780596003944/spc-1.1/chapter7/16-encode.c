#include <openssl/rsa.h>

/* Returns the malloc'd buffer, and puts the size of the buffer into the integer
 * pointed to by the second argument.
 */
unsigned char *DER_encode_RSA_public(RSA *rsa, int *len) {
  unsigned char *buf, *next;

  *len = i2d_RSAPublicKey(rsa, 0);
  if (!(buf = next = (unsigned char *)malloc(*len))) return 0;
  i2d_RSAPublicKey(rsa, &next); /* If we use buf here, return buf; becomes wrong */
  return buf;
}
