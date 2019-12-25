void spc_cipherq_async_setup(SPC_CIPHERQ *q1, SPC_CIPHERQ *q2) {
  memcpy(q2, q1, sizeof(SPC_CIPHERQ));
  q1->nonce[0] &= 0x7f;  /* The upper bit of q1's nonce is always 0. */
  q2->nonce[0] |= 0x80;  /* The upper bit of q2's nonce is always 1. */
}
