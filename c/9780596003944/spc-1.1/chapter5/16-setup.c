#include <stdlib.h>
#include <string.h>
#include <cwc.h>

#define MAX_KEY_LEN (32)  /* 256 bits */

size_t spc_cipherq_setup(SPC_CIPHERQ *q, unsigned char *basekey, size_t keylen,
                         size_t keyuses) {
  unsigned char dk[MAX_KEY_LEN];
  unsigned char salt[5];

  spc_rand(salt, 5);
  spc_make_derived_key(basekey, keylen, salt, 5, 1, dk, keylen);
  if (!cwc_init(&(q->ctx), dk, keylen * 8)) return 0;
  memcpy(q->nonce, salt, 5);
  spc_memset(basekey, 0, keylen);
  return keyuses + 1;
}
