int             fingerprint_length;
SSL             *ssl;
EVP_MD          *digest;
SSL_CTX         *ctx;
unsigned char   fingerprint[EVP_MAX_MD_SIZE];
spc_x509store_t spc_store;

spc_init_x509store(&spc_store);
spc_x509store_setcallback(&spc_store, spc_whitelist_callback);
spc_x509store_setflags(&spc_store, SPC_X509STORE_SSL_VERIFY_PEER |
 SPC_X509STORE_SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
ctx = spc_create_sslctx(&spc_store);
/* use the ctx to establish a connection.  This will yield an SSL object */
cert = SSL_get_peer_certificate(ssl);
digest = EVP_sha1();
fingerprint_length = sizeof(fingerprint);
spc_fingerprint_cert(cert, digest, fingerprint, &fingerprint_length);
/* use the fingerprint to compare against the list of known cert fingerprints */
