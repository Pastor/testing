#include <stdlib.h>

static char b64table[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz"
                           "0123456789+/";

/* Accepts a binary buffer with an associated size.
 * Returns a base64-encoded, NULL-terminated string.
 */
unsigned char *spc_base64_encode(unsigned char *input, size_t len, int wrap) {
  unsigned char *output, *p;
  size_t        i = 0, mod = len % 3, toalloc;

  toalloc = (len / 3) * 4 + (3 - mod) % 3 + 1;
  if (wrap) {
    toalloc += len / 57;
    if (len % 57) toalloc++;
  }

  if (toalloc < len) return 0;
  p = output = (unsigned char *)malloc(toalloc);
  if (!p) return 0;

  while (i < len - mod) {
    *p++ = b64table[input[i++] >> 2];
    *p++ = b64table[((input[i - 1] << 4) | (input[i] >> 4)) & 0x3f];
    *p++ = b64table[((input[i] << 2) | (input[i + 1] >> 6)) & 0x3f];
    *p++ = b64table[input[i + 1] & 0x3f];
    i += 2;
    if (wrap && !(i % 57)) *p++ = '\n';
  }
  if (!mod) {
    if (wrap && i % 57) *p++ = '\n';
    *p = 0;
    return output;
  } else {
    *p++ = b64table[input[i++] >> 2];
    *p++ = b64table[((input[i - 1] << 4) | (input[i] >> 4)) & 0x3f];
    if (mod == 1) {
      *p++ = '=';
      *p++ = '=';
      if (wrap) *p++ = '\n';
      *p = 0;
      return output;
    } else {
      *p++ = b64table[(input[i] << 2) & 0x3f];
      *p++ = '=';
      if (wrap) *p++ = '\n';
      *p = 0;
      return output;
    }
  }
}
