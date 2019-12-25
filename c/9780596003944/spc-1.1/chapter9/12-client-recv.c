static void spc_ssock_read(int, unsigned char *, size_t);
static void spc_get_status_and_nonce(int, unsigned char *, unsigned char *);
static unsigned char *spc_finish_decryption(spc_ssock_t *, unsigned char,
                                            unsigned char *, size_t *);

unsigned char *spc_client_read(spc_ssock_t *ctx,  size_t *len, size_t *end) {
  unsigned char status;
  unsigned char nonce[SPC_CWC_NONCE_LEN];

  /* If it's the client's turn to speak,  abort. */
  if (ctx->nonce[0] != SPC_CLIENT_DISTINGUISHER) abort();
  ctx->nonce[0] = SPC_SERVER_DISTINGUISHER;
  spc_get_status_and_nonce(ctx->fd, &status, nonce);
  *end = status;
  return spc_finish_decryption(ctx, status, nonce, len);
}

static void spc_get_status_and_nonce(int fd, unsigned char *status,
                                     unsigned char *nonce) {
  /* Read the status byte.  If it's 0x00 or 0xff, we're going to look at
   * the rest of the message, otherwise we'll just give up right away.
   */
  spc_ssock_read(fd,  status, 1);
  if (*status != spc_msg_ok && *status != spc_msg_end) abort( );
  spc_ssock_read(fd, nonce, SPC_CWC_NONCE_LEN);
}

static unsigned char *spc_finish_decryption(spc_ssock_t *ctx, unsigned char status,
                                            unsigned char *nonce, size_t *len) {
  size_t        ctlen = 0, i;
  unsigned char *ct, encoded_len[SPC_MLEN_FIELD_LEN];

  /* Check the nonce. */
  for (i = 0;  i < SPC_CWC_NONCE_LEN;  i++)
    if (nonce[i] != ctx->nonce[i]) abort();

  /* Read the length field. */
  spc_ssock_read(ctx->fd, encoded_len, SPC_MLEN_FIELD_LEN);
  for (i = 0;  i < SPC_MLEN_FIELD_LEN;  i++) {
    ctlen <<= 8;
    ctlen += encoded_len[i];
  }

  /* Read the ciphertext. */
  if (!(ct = (unsigned char *)malloc(ctlen))) abort(); /* Out of memory. */
  spc_ssock_read(ctx->fd, ct, ctlen);

  /* Decrypt the ciphertext, and abort if decryption fails.
   * We decrypt into the buffer in which the ciphertext already lives.
   */
  if (!cwc_decrypt_message(&(ctx->cwc), &status, 1, ct, ctlen, nonce, ct)) {
    free(ct);
    abort();
  }

  *len = ctlen - SPC_CWC_TAG_LEN;
  /* We'll go ahead and avoid the realloc(), leaving SPC_CWC_TAG_LEN extra
   * bytes at the end of the buffer than we need to leave.
   */
  return ct;
}

static void spc_ssock_read(int fd, unsigned char *msg, size_t mlen) {
  ssize_t r;

  while (mlen) {
    if ((r = read(fd, msg, mlen)) == -1) {
      switch (errno) {
        case EINTR:
          break;
        default:
          abort();
      }
    } else {
      mlen -= r;
      msg += r;
    }
  }
}
