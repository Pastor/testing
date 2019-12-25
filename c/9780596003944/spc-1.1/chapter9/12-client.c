/* keylen is in bytes.  Note that, on errors, we abort(), whereas you will
 * probably want to perform exception handling, as discussed in Recipe 13.1.
 * In any event, we never report an error to the other side; simply drop the
 * connection (by aborting).  We'll send a message when shutting down properly.
 */

void spc_init_client(spc_ssock_t *ctx, unsigned char *key, size_t klen, int fd) {
  if (klen != 16 && klen != 24 && klen != 32) abort();

  /* Remember that cwc_init() erases the key we pass in! */
  cwc_init(&(ctx->cwc), key, klen * 8);

  /* select 5 random bytes to place starting at nonce[1].  We use the API from
   * Recipe 11.2.
   */
  spc_rand(ctx->nonce + SPC_IV_IX, SPC_IV_LEN);

  /* Set the 5 counterbytes  to 0, indicating that we've sent no messages. */
  memset(ctx->nonce + SPC_CTR_IX, 0, SPC_CTR_LEN);
  ctx->fd = fd;

  /* This value always holds the value of the last person to send a message.
   * If the client goes to send a message, and this is sent to
   * SPC_CLIENT_DISTINGUISHER,  then we know there has been an error.
   */
  ctx->nonce[0] = SPC_SERVER_DISTINGUISHER;
}
