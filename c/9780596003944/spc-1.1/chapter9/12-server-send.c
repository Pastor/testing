void spc_ssock_server_send(spc_ssock_t *ctx, unsigned char *msg, size_t mlen) {
  /* If it's not our turn to speak, abort. We know it's our turn if the client
   * spoke last.
   */
  if (ctx->nonce[0] != SPC_CLIENT_DISTINGUISHER) abort();

  /* Set the distinguisher, but don't bump the counter, because we already did
   * when we received the message from the client.
   */
  ctx->nonce[0] = SPC_SERVER_DISTINGUISHER;
  spc_base_send(ctx, msg, mlen);
}
