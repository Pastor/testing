#include <stdlib.h>

void RC4_set_key(RC4_CTX *c, size_t keybytes, unsigned char *key) {
  int           i, j;
  unsigned char keyarr[256], swap;

  c->i = c->j = 0;
  for (i = j = 0;  i < 256;  i++, j = (j + 1) % keybytes) {
    c->sbox[i] = i;
    keyarr[i] = key[j];
  }
  for (i = j = 0;  i < 256;  i++) {
    j += c->sbox[i] + keyarr[i];
    j %= 256;
    swap = c->sbox[i];
    c->sbox[i] = c->sbox[j];
    c->sbox[j] = swap;
  }
}
