#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

static int read_data(int fd, void *buf, size_t nbytes) {
  size_t  toread, nread = 0;
  ssize_t result;

  do {
    if (nbytes - nread > SSIZE_MAX) toread = SSIZE_MAX;
    else toread = nbytes - nread;
    if ((result = read(fd, (char *)buf + nread, toread)) >= 0)
      nread += result;
    else if (errno != EINTR) return 0;
  } while (nread < nbytes);
  return 1;
}

static int write_data(int fd, const void *buf, size_t nbytes) {
  size_t  towrite, written = 0;
  ssize_t result;

  do {
    if (nbytes - written > SSIZE_MAX) towrite = SSIZE_MAX;
    else towrite = nbytes - written;
    if ((result = write(fd, (const char *)buf + written, towrite)) >= 0)
      written += result;
    else if (errno != EINTR) return 0;
  } while (written < nbytes);
  return 1;
}

int spc_lock_file(const char *lfpath) {
  int   attempt, fd, result;
  pid_t pid;

  /* Try three times, if we fail that many times, we lose */
  for (attempt = 0;  attempt < 3;  attempt++) {
    if ((fd = open(lfpath, O_RDWR | O_CREAT | O_EXCL, S_IRWXU)) == -1) {
      if (errno != EEXIST) return -1;
      if ((fd = open(lfpath, O_RDONLY)) == -1) return -1;
      result = read_data(fd, &pid, sizeof(pid));
      close(fd);
      if (result) {
        if (pid == getpid(  )) return 1;
        if (kill(pid, 0) == -1) {
          if (errno != ESRCH) return -1;
          attempt--;
          unlink(lfpath);
          continue;
        }
      }
      sleep(1);
      continue;
    }

    pid = getpid();
    if (!write_data(fd, &pid, sizeof(pid))) {
      close(fd);
      return -1;
    }
    close(fd);
    attempt--;
  }

  /* If we've made it to here, three attempts have been made and the lock could
   * not be obtained.  Return an error code indicating failure to obtain the
   * requested lock.
   */
  return 0;
}
