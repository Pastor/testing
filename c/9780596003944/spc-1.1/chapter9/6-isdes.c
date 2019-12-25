#include <krb5.h>

int spc_krb5_isdes(krb5_keyblock *key) {
#ifdef KRB5_GENERAL__
  if (key->enctype == ENCTYPE_DES_CBC_CRC || key->enctype == ENCTYPE_DES_CBC_MD4 ||
      key->enctype == ENCTYPE_DES_CBC_MD5 || key->enctype == ENCTYPE_DES_CBC_RAW)
    return 1;
#else
  if (key->keytype == ETYPE_DES_CBC_CRC || key->keytype == ETYPE_DES_CBC_MD4 ||
      key->keytype == ETYPE_DES_CBC_MD5 || key->keytype == ETYPE_DES_CBC_NONE ||
      key->keytype == ETYPE_DES_CFB64_NONE || key->keytype == ETYPE_DES_PCBC_NONE)
    return 1;
#endif
  return 0;
}
