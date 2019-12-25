#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <windows.h>
#include <winsock.h>
#endif

/* This implementation only works when the block size is equal to the key size */

typedef struct {
  unsigned char h1[SPC_BLOCK_SZ];
  unsigned char h2[SPC_BLOCK_SZ];
  unsigned char bf[SPC_BLOCK_SZ];
  size_t        ix;
  size_t        tl;
} SPC_MDC2_CTX;

void spc_mdc2_init(SPC_MDC2_CTX *c) {
  memset(c->h1, 0x52, SPC_BLOCK_SZ);
  memset(c->h2, 0x25, SPC_BLOCK_SZ);
  c->ix = 0;
  c->tl = 0;
}

static void spc_mdc2_oneblock(SPC_MDC2_CTX *c, unsigned char bl[SPC_BLOCK_SZ]) {
  int           i, j;
  SPC_KEY_SCHED ks1, ks2;

  SPC_ENCRYPT_INIT(&ks1, c->h1, SPC_BLOCK_SZ);
  SPC_ENCRYPT_INIT(&ks2, c->h2, SPC_BLOCK_SZ);
  SPC_DO_ENCRYPT(&ks1, bl, c->h1);
  SPC_DO_ENCRYPT(&ks2, bl, c->h2);
  j = SPC_BLOCK_SZ / (sizeof(int) * 2);
  for (i = 0;  i < SPC_BLOCK_SZ / (sizeof(int) * 2);  i++) {
    ((int *)c->h1)[i]     ^= ((int *)bl)[i];
    ((int *)c->h2)[i]     ^= ((int *)bl)[i];
    ((int *)c->h1)[i + j] ^= ((int *)bl)[i + j];
    ((int *)c->h2)[i + j] ^= ((int *)bl)[i + j];
    /* Now swap the lower halves using XOR. */
    ((int *)c->h1)[i + j] ^= ((int *)c->h2)[i + j];
    ((int *)c->h2)[i + j] ^= ((int *)c->h1)[i + j];
    ((int *)c->h1)[i + j] ^= ((int *)c->h2)[i + j];
  }
}

void spc_mdc2_update(SPC_MDC2_CTX *c, unsigned char *t, size_t l) {
  c->tl += l;  /* if c->tl < l: abort */
  while (c->ix && l) {
    c->bf[c->ix++] = *t++;
    l--;
    if (!(c->ix %= SPC_BLOCK_SZ))
      spc_mdc2_oneblock(c, c->bf);
  }
  while (l > SPC_BLOCK_SZ) {
    spc_mdc2_oneblock(c, t);
    t += SPC_BLOCK_SZ;
    l -= SPC_BLOCK_SZ;
  }
  c->ix = l;
  for (l = 0;  l < c->ix;  l++)
    c->bf[l] = *t++;
}

void spc_mdc2_final(SPC_MDC2_CTX *c, unsigned char output[SPC_BLOCK_SZ * 2]) {
  int i;

  c->bf[c->ix++] = 0x80;
  while (c->ix < SPC_BLOCK_SZ)
    c->bf[c->ix++] = 0;
  spc_mdc2_oneblock(c, c->bf);
  memset(c->bf, 0, SPC_BLOCK_SZ - sizeof(size_t));
  c->tl = htonl(c->tl);
  for (i = 0;  i < sizeof(size_t);  i++)
    c->bf[SPC_BLOCK_SZ - sizeof(size_t) + i] = ((unsigned char *)(&c->tl))[i];
  spc_mdc2_oneblock(c, c->bf);
  memcpy(output, c->h1, SPC_BLOCK_SZ);
  memcpy(output+SPC_BLOCK_SZ, c->h2, SPC_BLOCK_SZ);
}
