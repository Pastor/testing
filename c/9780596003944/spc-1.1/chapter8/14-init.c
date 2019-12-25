#include <stdlib.h>
#include <string.h>
#include <cwc.h>

static cwc_t spc_cookie_cwc;
static unsigned char spc_cookie_nonce[11];

int spc_cookie_init(unsigned char *key, size_t keylen) {
  memset(spc_cookie_nonce, 0, sizeof(spc_cookie_nonce));
  return cwc_init(&spc_cookie_cwc, key, keylen * 8);
}
