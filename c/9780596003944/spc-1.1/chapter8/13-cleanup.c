void spc_krb5_cleanup(spc_krb5bundle_t *bundle) {
  if (bundle->ticket) {
    krb5_free_ticket(bundle->ctx, bundle->ticket);
    bundle->ticket = 0;
  }
  if (bundle->auth_ctx) {
    krb5_auth_con_free(bundle->ctx, bundle->auth_ctx);
    bundle->auth_ctx = 0;
  }
  if (bundle->ctx) {
    krb5_free_context(bundle->ctx);
    bundle->ctx = 0;
  }
}
