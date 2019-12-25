typedef struct {
  SPC_KEY_SCHED ks;
  int           ix;
  unsigned char nonce[SPC_BLOCK_SZ];
} SPC_OFB_CTX;
