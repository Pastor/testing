#include <stdlib.h>
#include <string.h>

#define RNG_BLOCK_SZ 16

char *spc_fips_entropy(char *outbuf, int n) {
  static int  i, bufsz = -1;
  static char b1[RNG_BLOCK_SZ], b2[RNG_BLOCK_SZ];
  static char *last = b1, *next = b2;
  char        *p = outbuf;

  if (bufsz == -1) {
    spc_entropy(next, RNG_BLOCK_SZ);
    bufsz = 0;
  }
  while (bufsz && n--)
    *p++ = last[RNG_BLOCK_SZ - bufsz--];
  while (n >= RNG_BLOCK_SZ) {
    /* Old next becomes last here */
    *next ^= *last;
    *last ^= *next;
    *next ^= *last;
    spc_entropy(next, RNG_BLOCK_SZ);
    for (i = 0;  i < RNG_BLOCK_SZ;  i++)
      if (next[i] != last[i]) goto okay;
    abort();
okay:
    memcpy(p, next, RNG_BLOCK_SZ);
    p += RNG_BLOCK_SZ;
    n -= RNG_BLOCK_SZ;
  }
  if (n) {
    *next ^= *last;
    *last ^= *next;
    *next ^= *last;
    spc_entropy(next, RNG_BLOCK_SZ);
    for (i = 0;  i < RNG_BLOCK_SZ;  i++)
      if (next[i] != last[i])
         goto okay2;
    abort();
okay2:
    memcpy(p, next, n);
    bufsz = RNG_BLOCK_SZ - n;
  }
  return outbuf;
}
