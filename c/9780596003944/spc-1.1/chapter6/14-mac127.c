#include <stdlib.h>
#ifndef WIN32
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <windows.h>
#include <winsock.h>
#endif
#include <hash127.h>

typedef struct {
  struct hash127 hctx;
  SPC_KEY_SCHED  ekey;
  SPC_KEY_SCHED  nkey;
} SPC_MAC127_CTX;

void spc_mac127_init(SPC_MAC127_CTX *ctx, unsigned char key[16]) {
  int                    i;
  unsigned char          pt[16] = {0, };
  volatile int32         hk[4];
  volatile unsigned char ek[16], nk[16];

  SPC_ENCRYPT_INIT(&(ctx->ekey), key, 16);
  SPC_DO_ENCRYPT(&(ctx->ekey), pt, (unsigned char *)ek);
  pt[15] = 1;
  SPC_DO_ENCRYPT(&(ctx->ekey), pt, (unsigned char *)nk);
  pt[15] = 2;
  SPC_DO_ENCRYPT(&(ctx->ekey), pt, (unsigned char *)hk);
  SPC_ENCRYPT_INIT(&(ctx->ekey), (unsigned char *)ek, 16);
  SPC_ENCRYPT_INIT(&(ctx->nkey), (unsigned char *)nk, 16);
  hk[0] = htonl(hk[0]);
  hk[1] = htonl(hk[1]);
  hk[2] = htonl(hk[2]);
  hk[3] = htonl(hk[3]);
  hash127_expand(&(ctx->hctx), (int32 *)hk);
  hk[0] = hk[1] = hk[2] = hk[3] = 0;
  for (i = 0;  i < 16;  i++) ek[i] = nk[i] = 0;
}

void spc_mac127(SPC_MAC127_CTX *c, unsigned char *msg, size_t mlen,
                unsigned char nonce[16], unsigned char out[16]) {
  int   i, r = mlen % 4; /* leftover bytes to stick into final block */
  int32 x[4] = {0,};

  for (i = 0;  i <r;  i++) ((unsigned char *)x)[i] = msg[mlen - r + i];
  x[3] = (int32)mlen;
  hash127_little((int32 *)out, (int32 *)msg, mlen / 4, &(c->hctx), x);
  x[0] = htonl(*(int *)out);
  x[1] = htonl(*(int *)(out + 4));
  x[2] = htonl(*(int *)(out + 8));
  x[3] = htonl(*(int *)(out + 12));
  SPC_DO_ENCRYPT(&(c->ekey), out, out);
  SPC_DO_ENCRYPT(&(c->nkey), nonce, (unsigned char *)x);
  ((int32 *)out)[0] ^= x[0];
  ((int32 *)out)[1] ^= x[1];
  ((int32 *)out)[2] ^= x[2];
  ((int32 *)out)[3] ^= x[3];
}
