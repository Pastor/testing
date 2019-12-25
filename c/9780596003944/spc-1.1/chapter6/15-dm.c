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

#define SPC_KEY_SZ 16

typedef struct {
  unsigned char h[SPC_BLOCK_SZ];
  unsigned char b[SPC_KEY_SZ];
  size_t        ix;
  size_t        tl;
} SPC_DM_CTX;

void spc_dm_init(SPC_DM_CTX *c) {
  memset(c->h, 0x52, SPC_BLOCK_SZ);
  c->ix = 0;
  c->tl = 0;
}

static void spc_dm_once(SPC_DM_CTX *c, unsigned char b[SPC_KEY_SZ]) {
  int           i;
  SPC_KEY_SCHED ks;
  unsigned char tmp[SPC_BLOCK_SZ];

  SPC_ENCRYPT_INIT(&ks, b, SPC_KEY_SZ);
  SPC_DO_ENCRYPT(&ks, c->h, tmp);
  for (i = 0;  i < SPC_BLOCK_SZ / sizeof(int);  i++)
    ((int *)c->h)[i] ^= ((int *)tmp)[i];
}

void spc_dm_update(SPC_DM_CTX *c, unsigned char *t, size_t l) {
  c->tl += l;  /* if c->tl < l: abort */
  while (c->ix && l) {
    c->b[c->ix++] = *t++;
    l--;
    if (!(c->ix %= SPC_KEY_SZ)) spc_dm_once(c, c->b);
  }
  while (l > SPC_KEY_SZ) {
    spc_dm_once(c, t);
    t += SPC_KEY_SZ;
    l -= SPC_KEY_SZ;
  }
  c->ix = l;
  for (l = 0;  l < c->ix;  l++) c->b[l] = *t++;
}

void spc_dm_final(SPC_DM_CTX *c, unsigned char output[SPC_BLOCK_SZ]) {
  int i;

  c->b[c->ix++] = 0x80;
  while (c->ix < SPC_KEY_SZ) c->b[c->ix++] = 0;
  spc_dm_once(c, c->b);
  memset(c->b, 0, SPC_KEY_SZ - sizeof(size_t));
  c->tl = htonl(c->tl);
  for (i = 0;  i < sizeof(size_t);  i++)
    c->b[SPC_KEY_SZ - sizeof(size_t) + i] = ((unsigned char *)(&c->tl))[i];
  spc_dm_once(c, c->b);
  memcpy(output, c->h, SPC_BLOCK_SZ);
}
