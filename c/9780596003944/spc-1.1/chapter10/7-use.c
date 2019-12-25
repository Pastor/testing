void spc_x509store_setusecertfile(spc_x509store_t *spc_store, char *file) {
  if (spc_store->use_certfile) free(spc_store->use_certfile);
  spc_store->use_certfile = (file ? strdup(file) : 0);
}

void spc_x509store_addusecert(spc_x509store_t *spc_store, X509 *cert) {
  sk_X509_push(spc_store->certs, cert);
}

void spc_x509store_setusekeyfile(spc_x509store_t *spc_store, char *file) {
  if (spc_store->use_keyfile) free(spc_store->use_keyfile);
  spc_store->use_keyfile = (file ? strdup(file) : 0);
}

void spc_x509store_setusekey(spc_x509store_t *spc_store, EVP_PKEY *key) {
  if (spc_store->use_key) EVP_PKEY_free(key);
  spc_store->use_key = key;
  CRYPTO_add(&(key->references), 1, CRYPTO_LOCK_EVP_PKEY);
}
