#include <stddef.h>

volatile void *spc_memset(volatile void *dst, int c, size_t len) {
  volatile char *buf;

  for (buf = (volatile char *)dst;  len;  buf[--len] = c);
  return dst;
}

volatile void *spc_memcpy(volatile void *dst, volatile void *src, size_t len) {
  volatile char *cdst, *csrc;

  cdst = (volatile char *)dst;
  csrc = (volatile char *)src;
  while (len--) cdst[len] = csrc[len];
  return dst;
}

volatile void *spc_memmove(volatile void *dst, volatile void *src, size_t len) {
  size_t        i;
  volatile char *cdst, *csrc;

  cdst = (volatile char *)dst;
  csrc = (volatile char *)src;
  if (csrc > cdst && csrc < cdst + len)
    for (i = 0;  i < len;  i++) cdst[i] = csrc[i];
  else
    while (len--) cdst[len] = csrc[len];
  return dst;
}
