#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

int spc_mysql_real_connect(MYSQL *mysql, const char *host, const char *pw,
                           const char *db, unsigned int flags) {
  int         port = 0, result = 0;
  char        *host_copy = 0, *p;
  const char  *socket = 0, *user = 0;

  if (host) {
    if (!(host_copy = strdup(host))) return 0;
    if ((p = strchr(host_copy, '@')) != 0) {
      user = host_copy;
      *p++ = 0;
      host = p;
    }
    if ((p = strchr((p ? p : host_copy), ':')) != 0) {
      *p++ = 0;
      port = atoi(p);
    }
    if (*host == '/') {
      socket = host;
      host = 0;
    }
  }

  /* this bit of magic is all it takes to enable SSL connections */
  flags |= CLIENT_SSL;

  if (mysql_real_connect(mysql, host, user, pw, db, port, socket, flags))
    result = 1;
  if (host_copy) free(host_copy);
  return result;
}
