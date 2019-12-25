#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

char *spc_cert_filename(char *path, X509 *cert) {
  int  length;
  char *filename;

  length = strlen(path) + 11;
  if (!(filename = (char *)malloc(length + 1))) return 0;
  snprintf(filename, length + 1, "%s/%08lx.0", path, X509_subject_name_hash(cert));
  return filename;
}

int spc_remember_cert(char *path, X509 *cert) {
  int  result;
  char *filename;
  FILE *fp;

  if (!(filename = spc_cert_filename(path, cert))) return 0;
  if (!(fp = fopen(filename, "w"))) {
    free(filename);
    return 0;
  }
  result = PEM_write_X509(fp, cert);
  fclose(fp);
  if (!result) remove(filename);
  free(filename);
  return result;
}

int spc_verifyandmaybesave_callback(int ok, X509_STORE_CTX *store) {
  int             err;
  SSL             *ssl_ptr;
  char            answer[80], name[256];
  X509            *cert;
  SSL_CTX         *ctx;
  spc_x509store_t *spc_store;

  if (ok) return ok;

  cert = X509_STORE_CTX_get_current_cert(store);
  printf("An error has occurred with the following certificate:\n");
  X509_NAME_oneline(X509_get_issuer_name(cert), name, sizeof(name));
  printf("    Issuer Name:  %s\n", name);
  X509_NAME_oneline(X509_get_subject_name(cert), name, sizeof(name));
  printf("    Subject Name: %s\n", name);
  err = X509_STORE_CTX_get_error(store);
  printf("    Error Reason: %s\n", X509_verify_cert_error_string(err));
  for (;;) {
    printf("Do you want to [r]eject this certificate, [a]ccept and remember it, "
           "or allow\nits use for only this [o]ne time? ");
    if (!fgets(answer, sizeof(answer), stdin)) continue;

    if (answer[0] == 'r' || answer[0] == 'R') return 0;
    if (answer[0] == 'o' || answer[0] == 'O') return 1;
    if (answer[0] == 'a' || answer[0] == 'A') break;
  }

  ssl_ptr = (SSL *)X509_STORE_CTX_get_app_data(store);
  ctx = SSL_get_SSL_CTX(ssl_ptr);
  spc_store = (spc_x509store_t *)SSL_CTX_get_app_data(ctx);
  if (!spc_store->capath || !spc_remember_cert(spc_store->capath, cert))
    printf("Error remembering certificate!  It will be accepted this one time "
           "only.\n");
  return 1;
}
