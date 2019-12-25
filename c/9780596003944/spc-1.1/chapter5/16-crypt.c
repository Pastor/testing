static void increment_counter(SPC_CIPHERQ *q) {
  if (!++q->nonce[10]) if (!++q->nonce[9]) if (!++q->nonce[8]) if (!++q->nonce[7])
    if (!++q->nonce[6]) ++q->nonce[5];
}

unsigned char *spc_cipherq_encrypt(SPC_CIPHERQ *q, unsigned char *m, size_t mlen,
                                   size_t *ol) {
  unsigned char *ret;

  if (mlen + 16 < 16) return 0;
  if (!(ret = (unsigned char *)malloc(mlen + 16))) {
    if (ol) *ol = 0;
    return 0;
  }
  cwc_encrypt(&(q->ctx), 0, 0, m, mlen, q->nonce, ret);
  increment_counter(q);
  if (ol) *ol = mlen + 16;
  return ret;
}

unsigned char *spc_cipherq_decrypt(SPC_CIPHERQ *q, unsigned char *m, size_t mlen,
                                   size_t *ol) {
  unsigned char *ret;

  if (mlen < 16) return 0;
  if (!(ret = (unsigned char *)malloc(mlen - 16))) {
    if (ol) *ol = 0;
    return 0;
  }
  if (!cwc_decrypt(&(q->ctx), 0, 0, m, mlen, q->nonce, ret)) {
    free(ret);
    if (ol) *ol = 0;
    return 0;
  }
  increment_counter(q);
  if (ol) *ol = mlen - 16;
  return ret;
}
