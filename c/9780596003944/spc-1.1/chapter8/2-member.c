#include <sys/types.h>
#include <grp.h>
#include <string.h>

int spc_group_ismember(char *group_name, char *user_name) {
  int          i;
  struct group *gr;

  if (!(gr = getgrnam(group_name))) {
    endgrent();
    return 0;
  }

  for (i = 0;  gr->gr_mem[i];  i++)
    if (!strcmp(user_name, gr->gr_mem[i])) {
      endgrent();
      return 1;
    }

  endgrent();
  return 0;
}
