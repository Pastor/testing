void spc_init_x509store(spc_x509store_t *spc_store) {
  spc_store->cafile       = 0;
  spc_store->capath       = 0;
  spc_store->crlfile      = 0;
  spc_store->callback     = 0;
  spc_store->certs        = sk_X509_new_null();
  spc_store->crls         = sk_X509_CRL_new_null();
  spc_store->use_certfile = 0;
  spc_store->use_certs    = sk_X509_new_null();
  spc_store->use_keyfile  = 0;
  spc_store->use_key      = 0;
  spc_store->flags        = 0;
}

void spc_cleanup_x509store(spc_x509store_t *spc_store) {
  if (spc_store->cafile)       free(spc_store->cafile);
  if (spc_store->capath)       free(spc_store->capath);
  if (spc_store->crlfile)      free(spc_store->crlfile);
  if (spc_store->use_certfile) free(spc_store->use_certfile);
  if (spc_store->use_keyfile)  free(spc_store->use_keyfile);
  if (spc_store->use_key) EVP_PKEY_free(spc_store->use_key);
  sk_X509_free(spc_store->certs);
  sk_X509_free(spc_store->crls);
  sk_X509_free(spc_store->use_certs);
}
