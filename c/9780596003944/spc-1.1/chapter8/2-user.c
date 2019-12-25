#include <sys/types.h>
#include <pwd.h>
#include <string.h>

int spc_user_getname(uid_t uid, char **name) {
  struct passwd *pw;

  if (!(pw = getpwuid(uid)) ) {
    endpwent();
    return -1;
  }
  *name = strdup(pw->pw_name);
  endpwent();
  return 0;
}

int spc_user_getuid(char *name, uid_t *uid) {
  struct passwd *pw;

  if (!(pw = getpwnam(name))) {
    endpwent();
    return -1;
  }
  *uid = pw->pw_uid;
  endpwent();
  return 0;
}
