X509_CRL *spc_retrieve_crl(X509 *cert, X509 *issuer, spc_x509store_t *store) {
  BIO       *bio = 0;
  int       datalen, our_store;
  char      *uri = 0, *uri2 = 0;
  void      *data = 0;
  X509_CRL  *crl = 0;

  if ((our_store = (!store)) != 0) {
    if (!(store = (spc_x509store_t *)malloc(sizeof(spc_x509store_t)))) return 0;
    spc_init_x509store(store);
    spc_x509store_addcert(store, issuer);
  }
  if (!(uri = spc_getcert_crlurl(cert, issuer, 0))) goto end;
  if (!(data = retrieve_webdata(uri, &datalen, store))) {
    uri2 = spc_getcert_crlurl(cert, issuer, 1);
    if (!uri2 || !strcmp(uri, uri2)) goto end;
    if (!(data = retrieve_webdata(uri2, &datalen, store))) goto end;
  }

  bio = BIO_new_mem_buf(data, datalen);
  crl = d2i_X509_CRL_bio(bio, 0);

end:
  if (bio) BIO_free(bio);
  if (data) free(data);
  if (uri) free(uri);
  if (uri2) free(uri2);
  if (store && our_store) {
    spc_cleanup_x509store(store);
    free(store);
  }
  return crl;
}
