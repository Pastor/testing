unsigned char *spc_server_read(spc_ssock_t *ctx,  size_t *len, size_t *end) {
  unsigned char nonce[SPC_CWC_NONCE_LEN], status;

  /* If it's the server's turn to speak, abort. We know it's the server's turn
   * to speak if the first byte of the nonce is the CLIENT distinguisher.
   */
  if (ctx->nonce[0] != SPC_SERVER_DISTINGUISHER &&
      ctx->nonce[0] != SPC_SERVER_LACKS_NONCE) abort();

  spc_get_status_and_nonce(ctx->fd, &status, nonce);
  *end = status;

  /* If we need to do so, copy over the random bytes of the nonce. */
  if (ctx->nonce[0] == SPC_SERVER_LACKS_NONCE)
    memcpy(ctx->nonce + SPC_IV_IX, nonce + SPC_IV_IX, SPC_IV_LEN);

  /* Now, set the distinguisher field to client, and increment our copy of
   * the nonce.
   */
  ctx->nonce[0] = SPC_CLIENT_DISTINGUISHER;
  spc_increment_counter(ctx->nonce + SPC_CTR_IX, SPC_CTR_LEN);

  return spc_finish_decryption(ctx, status, nonce, len);
}
