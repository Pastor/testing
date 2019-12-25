#include <string.h>
#include <stdlib.h>
#include "wordlist.h"

#define BITS_IN_LIST 11
#define MAX_WORDLEN  4

/* len parameter is measured in bytes.  Remaining bits padded with 0. */
unsigned char *spc_bin2words(const unsigned char *str, size_t len) {
  short         add_space = 0;
  size_t        i, leftbits, leftovers, scratch = 0, scratch_bits = 0;
  unsigned char *p, *res;

  res = (unsigned char *)malloc(i = (len * 8 / BITS_IN_LIST + 1) * (MAX_WORDLEN + 1));
  if (!res || i<len) abort();
  res[0] = 0;

  for (i = 0;  i < len;  i++) {
    leftovers = str[i];
    leftbits = 8;
    while (leftbits) {
      if (scratch_bits + leftbits <= BITS_IN_LIST) {
        scratch |= (leftovers << (BITS_IN_LIST - leftbits - scratch_bits));
        scratch_bits += leftbits;
        leftbits = 0;
      } else {
        scratch |= (leftovers >> (leftbits - (BITS_IN_LIST - scratch_bits)));
        leftbits -= (BITS_IN_LIST - scratch_bits);
        leftovers &= ((1 << leftbits) - 1);
        scratch_bits = BITS_IN_LIST;
      }
      if (scratch_bits == BITS_IN_LIST) {
        p = words[scratch];
        /* The strcats are a bit inefficient because they start from the front of
         * the string each time.  But, they're less confusing, and these strings
         * should never get more than a few words long, so efficiency will
         * probably never be a real concern.
         */
        if (add_space) strcat(res, " ");
        strcat(res, p);
        scratch = scratch_bits = 0;
        add_space = 1;
      }
    }
  }
  if (scratch_bits) { /* Emit the final word */
    p = words[scratch];
    if (add_space) strcat(res, " ");
    strcat(res, p);
  }
  res = (unsigned char *)realloc(res, strlen(res) + 1);
  if (!res) abort(); /* realloc failed; should never happen, as size shrinks */
  return res;
}
