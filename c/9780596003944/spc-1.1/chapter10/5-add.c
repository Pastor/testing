void spc_x509store_addcert(spc_x509store_t *spc_store, X509 *cert) {
  sk_X509_push(spc_store->certs, cert);
}

void spc_x509store_addcrl(spc_x509store_t *spc_store, X509_CRL *crl) {
  sk_X509_CRL_push(spc_store->crls, crl);
}
