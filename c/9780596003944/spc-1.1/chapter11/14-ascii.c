#include <stdlib.h>

char *spc_rand_ascii(char *buf, size_t len) {
 char *p = buf;

 while (--len)
    *p++ = (char)spc_rand_range(33, 126);
  *p = 0;
  return buf;
} 
