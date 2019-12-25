void spc_init_server(spc_ssock_t *ctx, unsigned char *key, size_t klen,  int fd) {
  if (klen != 16 && klen != 24 && klen != 32) abort();

  /* Remember that cwc_init() erases the key we pass in! */
  cwc_init(&(ctx->cwc), key, klen * 8);

  /* We need to wait for the random portion of the nonce from the client.
   * The counter portion we can initialize to zero.  We'll set the distinguisher
   * to SPC_SERVER_LACKS_NONCE, so that we know to copy in the random portion
   * of the nonce when we receive a message.
   */
  ctx->nonce[0] = SPC_SERVER_LACKS_NONCE;
  memset(ctx->nonce + SPC_CTR_IX, 0, SPC_CTR_LEN);
  ctx->fd = fd;
}
