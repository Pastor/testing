#define SPC_CWC_TAG_LEN     16
#define SPC_MLEN_FIELD_LEN  4
#define SPC_MAX_MLEN        0xffffffff

static unsigned char spc_msg_ok  = 0x00;
static unsigned char spc_msg_end = 0xff;

static void spc_increment_counter(unsigned char *, size_t);
static void spc_ssock_write(int, unsigned char *, size_t);
static void spc_base_send(spc_ssock_t *ctx, unsigned char *msg, size_t mlen);

void spc_ssock_client_send(spc_ssock_t *ctx, unsigned char *msg, size_t mlen) {
  /* If it's not our turn to speak, abort. */
  if (ctx->nonce[0] != SPC_SERVER_DISTINGUISHER) abort();

  /* Set the distinguisher, then bump the counter before we actually send. */
  ctx->nonce[0] = SPC_CLIENT_DISTINGUISHER;
  spc_increment_counter(ctx->nonce + SPC_CTR_IX, SPC_CTR_LEN);
  spc_base_send(ctx, msg, mlen);
}

static void spc_base_send(spc_ssock_t *ctx, unsigned char *msg, size_t mlen) {
  unsigned char encoded_len[SPC_MLEN_FIELD_LEN];
  size_t        i;
  unsigned char *ct;

  /* If it's not our turn to speak, abort. */
  if (ctx->nonce[0] != SPC_SERVER_DISTINGUISHER) abort();

  /* First, write the status byte, then the nonce. */
  spc_ssock_write(ctx->fd, &spc_msg_ok, sizeof(spc_msg_ok));
  spc_ssock_write(ctx->fd, ctx->nonce, sizeof(ctx->nonce));

  /* Next, write the length of the ciphertext,
   * which will be the size of the plaintext plus SPC_CWC_TAG_LEN  bytes for
   * the tag.  We abort if the string is more than 2^32-1 bytes.
   * We do this in a way that is mostly oblivious to word size.
   */
  if (mlen > (unsigned long)SPC_MAX_MLEN ||  mlen < 0) abort( );
  for (i = 0;  i < SPC_MLEN_FIELD_LEN;   i++)
    encoded_len[SPC_MLEN_FIELD_LEN - i - 1] = (mlen >> (8 * i)) & 0xff;
  spc_ssock_write(ctx->fd, encoded_len, sizeof(encoded_len));

  /* Now, we perform the CWC encryption, and send the result. Note that,
   * if the send fails, and you do not abort as we do, you should remember to
   * deallocate the message buffer.
   */
  mlen += SPC_CWC_TAG_LEN;
  if (mlen < SPC_CWC_TAG_LEN) abort(); /* Message too long, mlen overflowed. */
  if (!(ct = (unsigned char *)malloc(mlen))) abort(); /* Out of memory. */
  cwc_encrypt_message(&(ctx->cwc),  &spc_msg_ok, sizeof(spc_msg_ok),  msg,
                        mlen - SPC_CWC_TAG_LEN, ctx->nonce, ct);
  spc_ssock_write(ctx->fd, ct, mlen);
  free(ct);
}

static void spc_increment_counter(unsigned char *ctr, size_t len) {
  while (len--) if (++ctr[len]) return;
  abort(); /* Counter rolled over, which is an error condition! */
}

static void spc_ssock_write( int fd, unsigned char *msg, size_t mlen) {
  ssize_t w;

  while (mlen) {
    if ((w = write(fd, msg, mlen)) == -1) {
      switch (errno) {
        case EINTR:
          break;
        default:
          abort();
      }
    } else {
      mlen -= w;
      msg += w;
    }
  }
}
