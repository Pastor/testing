#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int spc_is_safedir(const char *dir) {
  DIR         *fd, *start;
  int         rc = -1;
  char        new_dir[PATH_MAX + 1];
  uid_t       uid;
  struct stat f, l;

  if (!(start = opendir("."))) return -1;
  if (lstat(dir, &l) == -1) {
    closedir(start);
    return -1;
  }
  uid = geteuid();

  do {
    if (chdir(dir) == -1) break;
    if (!(fd = opendir("."))) break;
    if (fstat(dirfd(fd), &f) == -1) {
      closedir(fd);
      break;
    }
    closedir(fd);

    if (l.st_mode != f.st_mode || l.st_ino != f.st_ino || l.st_dev != f.st_dev)
      break;
    if ((f.st_mode & (S_IWOTH | S_IWGRP)) || (f.st_uid && f.st_uid != uid)) {
      rc = 0;
      break;
    }
    dir = "..";
    if (lstat(dir, &l) == -1) break;
    if (!getcwd(new_dir, PATH_MAX + 1)) break;
  } while (new_dir[1]); /* new_dir[0] will always be a slash */
  if (!new_dir[1]) rc = 1;

  fchdir(dirfd(start));
  closedir(start);
  return rc;
}
