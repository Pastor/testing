#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ERR_NOT_HEX  -1
#define ERR_BAD_SIZE -2
#define ERR_NO_MEM   -3

unsigned char *spc_hex2bin(const unsigned char *input, size_t *l) {
  unsigned char       shift = 4, value = 0;
  unsigned char       *r, *ret;
  const unsigned char *p;

  if (!(r = ret = (unsigned char *)malloc(strlen(input) / 2))) {
    *l = ERR_NO_MEM;
    return 0;
  }
  for (p = input;  isspace(*p);  p++);
  if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) p += 2;

  while (p[0]) {
    switch (p[0]) {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        value |= (*p++ - '0') << shift;
        break;
      case 'a': case 'b': case 'c':
      case 'd': case 'e': case 'f':
        value |= (*p++ - 'a' + 0xa) << shift;
        break;
      case 'A': case 'B': case 'C':
      case 'D': case 'E': case 'F':
        value |= (*p++ - 'A' + 0xa) << shift;
        break;
      case 0:
        if (!shift) {
          *l = ERR_NOT_HEX;
          free(ret);
          return 0;
        }
        break;
      default:
        if (isspace(p[0])) p++;
        else {
          *l = ERR_NOT_HEX;
          free(ret);
          return 0;
        }
    }
    if ((shift = (shift + 4) % 8) != 0) {
      *r++ = value;
      value = 0;
    }
  }
  if (!shift) {
    *l = ERR_BAD_SIZE;
    free(ret);
    return 0;
  }
  *l = (r - ret);
  return (unsigned char *)realloc(ret, *l);
}
