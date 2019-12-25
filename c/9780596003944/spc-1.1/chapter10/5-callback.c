void spc_x509store_setcallback(spc_x509store_t *spc_store,
                              spc_x509verifycallback_t callback) {
  spc_store->callback = callback;
}

#define SPC_X509STORE_NO_DEFAULT_CAFILE 0x01
#define SPC_X509STORE_NO_DEFAULT_CAPATH 0x02

void spc_x509store_setflags(spc_x509store_t *spc_store, int flags) {
  spc_store->flags |= flags;
}

void spc_x509store_clearflags(spc_x509store_t *spc_store, int flags) {
  spc_store->flags &= ~flags;
}

int spc_verify_callback(int ok, X509_STORE_CTX *store) {
  if (!ok)
    fprintf(stderr, "Error: %s\n", X509_verify_cert_error_string(store->error));
  return ok;
}
