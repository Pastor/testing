#include <stdlib.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>

BIO *spc_listen(char *host, int port) {
  BIO   *acpt = 0;
  int   addr_length;
  char  *addr;

  if (port < 1 || port > 65535) return 0;
  if (!host) host = "*";
  addr_length = strlen(host) + 6;  /* 5 for int, 1 for colon */
  if (!(addr = (char *)malloc(addr_length + 1))) return 0;
  snprintf(addr, addr_length + 1, "%s:%d", host, port);

  if ((acpt = BIO_new(BIO_s_accept())) != 0) {
    BIO_set_accept_port(acpt, addr);
    if (BIO_do_accept(acpt) <= 0) {
      BIO_free_all(acpt);
      acpt = 0;
    }
  }

  free(addr);
  return acpt;
}
