#include <openssl/ssl.h>

#define SPC_X509STORE_USE_CERTIFICATE                 0x04
#define SPC_X509STORE_SSL_VERIFY_NONE                 0x10
#define SPC_X509STORE_SSL_VERIFY_PEER                 0x20
#define SPC_X509STORE_SSL_VERIFY_FAIL_IF_NO_PEER_CERT 0x40
#define SPC_X509STORE_SSL_VERIFY_CLIENT_ONCE          0x80
#define SPC_X509STORE_SSL_VERIFY_MASK                 0xF0

SSL_CTX *spc_create_sslctx(spc_x509store_t *spc_store) {
  int                       i, verify_flags = 0;
  SSL_CTX                   *ctx = 0;
  X509_STORE                *store = 0;
  spc_x509verifycallback_t  verify_callback;

  if (!(ctx = SSL_CTX_new(SSLv23_method()))) goto error_exit;
  if (!(store = spc_create_x509store(spc_store))) goto error_exit;
  SSL_CTX_set_cert_store(ctx, store);  store = 0;
  SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2);
  SSL_CTX_set_cipher_list(ctx, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

  if (!(verify_callback = spc_store->callback))
    verify_callback = spc_verify_callback;
  if (!(spc_store->flags & SPC_X509STORE_SSL_VERIFY_MASK))
    verify_flags = SSL_VERIFY_NONE;
  else {
    if (spc_store->flags & SPC_X509STORE_SSL_VERIFY_NONE)
      verify_flags |= SSL_VERIFY_NONE;
    if (spc_store->flags & SPC_X509STORE_SSL_VERIFY_PEER)
      verify_flags |= SSL_VERIFY_PEER;
    if (spc_store->flags & SPC_X509STORE_SSL_VERIFY_FAIL_IF_NO_PEER_CERT)
      verify_flags |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
    if (spc_store->flags & SPC_X509STORE_SSL_VERIFY_CLIENT_ONCE)
      verify_flags |= SSL_VERIFY_CLIENT_ONCE;
  }
  SSL_CTX_set_verify(ctx, verify_flags, verify_callback);

  if (spc_store->flags & SPC_X509STORE_USE_CERTIFICATE) {
    if (spc_store->use_certfile)
      SSL_CTX_use_certificate_chain_file(ctx, spc_store->use_certfile);
    else {
      SSL_CTX_use_certificate(ctx, sk_X509_value(spc_store->use_certs, 0));
      for (i = 1; i < sk_X509_num(spc_store->use_certs); i++) {
        SSL_CTX_add_extra_chain_cert(ctx, sk_X509_value(spc_store->use_certs, i));
      }
    }
    if (spc_store->use_keyfile) {
      SSL_CTX_use_PrivateKey_file(ctx, spc_store->use_keyfile, SSL_FILETYPE_PEM);
    } else {
      if (spc_store->use_key)
        SSL_CTX_use_PrivateKey(ctx, spc_store->use_key);
    }
  }

  SSL_CTX_set_app_data(ctx, spc_store);
  return ctx;

error_exit:
  if (store) X509_STORE_free(store);  /* not ref counted */
  if (ctx) SSL_CTX_free(ctx);         /* ref counted */
  return 0;
}
