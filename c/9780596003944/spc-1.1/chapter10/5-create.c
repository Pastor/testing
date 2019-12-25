X509_STORE *spc_create_x509store(spc_x509store_t *spc_store) {
  int         i;
  X509_STORE  *store;
  X509_LOOKUP *lookup;

  store = X509_STORE_new();
  if (spc_store->callback)
    X509_STORE_set_verify_cb_func(store, spc_store->callback);
  else
    X509_STORE_set_verify_cb_func(store, spc_verify_callback);

  if (!(lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file())))
    goto error_exit;
  if (!spc_store->cafile) {
    if (!(spc_store->flags & SPC_X509STORE_NO_DEFAULT_CAFILE))
      X509_LOOKUP_load_file(lookup, 0, X509_FILETYPE_DEFAULT);
  } else if (!X509_LOOKUP_load_file(lookup, spc_store->cafile, X509_FILETYPE_PEM))
    goto error_exit;

  if (spc_store->crlfile) {
    if (!X509_load_crl_file(lookup, spc_store->crlfile, X509_FILETYPE_PEM))
      goto error_exit;
    X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK |
                                X509_V_FLAG_CRL_CHECK_ALL);
  }

  if (!(lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir())))
    goto error_exit;
  if (!spc_store->capath) {
    if (!(spc_store->flags & SPC_X509STORE_NO_DEFAULT_CAPATH))
      X509_LOOKUP_add_dir(lookup, 0, X509_FILETYPE_DEFAULT);
  } else if (!X509_LOOKUP_add_dir(lookup, spc_store->capath, X509_FILETYPE_PEM))
    goto error_exit;

  for (i = 0; i < sk_X509_num(spc_store->certs); i++)
    if (!X509_STORE_add_cert(store, sk_X509_value(spc_store->certs, i)))
      goto error_exit;
  for (i = 0; i < sk_X509_CRL_num(spc_store->crls); i++)
    if (!X509_STORE_add_crl(store, sk_X509_CRL_value(spc_store->crls, i)))
      goto error_exit;

  return store;

error_exit:
  if (store) X509_STORE_free(store);
  return 0;
}
