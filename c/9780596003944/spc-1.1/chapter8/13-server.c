krb5_error_code spc_krb5_server(int sockfd, spc_krb5bundle_t *bundle,
                                char *service, char *version) {
  int             free_context = 0;
  krb5_principal  server = 0;
  krb5_error_code rc;

  if (!bundle->ctx) {
    if ((rc = krb5_init_context(&(bundle->ctx))) != 0) goto error;
    free_context = 1;
  }
  if ((rc = krb5_sname_to_principal(bundle->ctx, 0, service,
                                    KRB5_NT_SRV_HST, &server)) != 0) goto error;

  rc = krb5_recvauth(bundle->ctx, &(bundle->auth_ctx), &sockfd, version,
                     server, 0, 0, &(bundle->ticket));
  if (!rc) {
    krb5_free_principal(bundle->ctx, server);
    return 0;
  }

error:
  if (server) krb5_free_principal(bundle->ctx, server);
  if (bundle->ctx && free_context) {
    krb5_free_context(bundle->ctx);
    bundle->ctx = 0;
  }
  return rc;
}
