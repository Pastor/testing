#include <openssl/bio.h>
#include <openssl/ssl.h>

void spc_enable_sessions(SSL_CTX *ctx, unsigned char *id, unsigned int id_len,
                         long timeout, int mode) {
  SSL_CTX_set_session_id_context(ctx, id, id_len);
  SSL_CTX_set_timeout(ctx, timeout);
  SSL_CTX_set_session_cache_mode(ctx, mode);
}
