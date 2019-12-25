typedef struct {
  SPC_KEY_SCHED ks;
  int           ix;
  int           pad;
  unsigned char iv[SPC_BLOCK_SZ];
  unsigned char ctbuf[SPC_BLOCK_SZ];
} SPC_CBC_CTX;
