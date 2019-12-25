#include <stdlib.h>
#include <openssl/bn.h>

#define BN_is_negative(x) ((x)->neg)

unsigned char *BN_to_binary(BIGNUM *b, unsigned int *outsz) {
  unsigned char *ret;

  *outsz = BN_num_bytes(b);
  if (BN_is_negative(b)) {
    (*outsz)++;
    if (!(ret = (unsigned char *)malloc(*outsz))) return 0;
    BN_bn2bin(b, ret + 1);
    ret[0] = 0x80;
  } else {
    if (!(ret = (unsigned char *)malloc(*outsz))) return 0;
    BN_bn2bin(b, ret);
  }
  return ret;
}
