#include <stdlib.h>

typedef struct {
  long int *fds_bits;
  size_t   fds_size;
} SPC_FD_SET;

void spc_fd_zero(SPC_FD_SET *fdset) {
  fdset->fds_bits = 0;
  fdset->fds_size = 0;
}

void spc_fd_set(int fd, SPC_FD_SET *fdset) {
  long   *tmp_bits;
  size_t new_size;

  if (fd < 0) return;
  if (fd > fdset->fds_size) {
    new_size = sizeof(long) * ((fd + sizeof(long) - 1) / sizeof(long));
    if (!(tmp_bits = (long *)realloc(fdset->fds_bits, new_size))) return;
    fdset->fds_bits = tmp_bits;
    fdset->fds_size = new_size;
  }
  fdset->fds_bits[fd / sizeof(long)] |= (1 << (fd % sizeof(long)));
}

void spc_fd_clr(int fd, SPC_FD_SET *fdset) {
  long   *tmp_bits;
  size_t new_size;

  if (fd < 0) return;
  if (fd > fdset->fds_size) {
    new_size = sizeof(long) * ((fd + sizeof(long) - 1) / sizeof(long));
    if (!(tmp_bits = (long *)realloc(fdset->fds_bits, new_size))) return;
    fdset->fds_bits = tmp_bits;
    fdset->fds_size = new_size;
  }
  fdset->fds_bits[fd / sizeof(long)] |= (1 << (fd % sizeof(long)));
}

int spc_fd_isset(int fd, SPC_FD_SET *fdset) {
  if (fd < 0 || fd >= fdset->fds_size) return 0;
  return (fdset->fds_bits[fd / sizeof(long)] & (1 << (fd % sizeof(long))));
}

void spc_fd_free(SPC_FD_SET *fdset) {
  if (fdset->fds_bits) free(fdset->fds_bits);
}

int spc_fd_setsize(SPC_FD_SET *fdset) {
  return fdset->fds_size;
}
