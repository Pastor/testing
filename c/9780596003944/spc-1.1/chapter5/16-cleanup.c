void spc_cipherq_cleanup(SPC_CIPHERQ *q) {
  spc_memset(q, 0, sizeof(SPC_CIPHERQ));
}
