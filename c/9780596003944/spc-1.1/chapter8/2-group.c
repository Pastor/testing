#include <sys/types.h>
#include <grp.h>
#include <string.h>

int spc_group_getname(gid_t gid, char **name) {
  struct group *gr;

  if (!(gr = getgruid(gid)) ) {
    endgrent();
    return -1;
  }
  *name = strdup(gr->gr_name);
  endgrent();
  return 0;
}

int spc_group_getgid(char *name, gid_t *gid) {
  struct group *gr;

  if (!(gr = getgrnam(name))) {
    endgrent();
    return -1;
  }
  *gid = gr->gr_gid;
  endgrent();
  return 0;
}
