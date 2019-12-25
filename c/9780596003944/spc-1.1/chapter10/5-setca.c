void spc_x509store_setcafile(spc_x509store_t *spc_store, char *cafile) {
  if (spc_store->cafile) free(spc_store->cafile);
  spc_store->cafile = (cafile ? strdup(cafile) : 0);
}

void spc_x509store_setcapath(spc_x509store_t *spc_store, char *capath) {
  if (spc_store->capath) free(spc_store->capath);
  spc_store->capath = (capath ? strdup(capath) : 0);
}

void spc_x509store_setcrlfile(spc_x509store_t *spc_store, char *crlfile) {
  if (spc_store->crlfile) free(spc_store->crlfile);
  spc_store->crlfile = (crlfile ? strdup(crlfile) : 0);
}
