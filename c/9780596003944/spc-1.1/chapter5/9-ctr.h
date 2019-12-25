typedef struct {
  SPC_KEY_SCHED ks;
  int           ix;
  unsigned char ctr[SPC_BLOCK_SZ];
  unsigned char ksm[SPC_BLOCK_SZ];
} SPC_CTR_CTX;
