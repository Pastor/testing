#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#ifndef TIOCGWINSZ
#include <sys/ioctl.h>
#endif
#include <openssl/sha.h>

#define HASH_OUT_SZ     20
#define OVERHEAD_CHARS  7
#define DEFAULT_BARSIZE (78 - OVERHEAD_CHARS)
#define MAX_BARSIZE     200

void spc_raw(int fd, struct termios *saved_opts) {
  struct termios new_opts;

  if (tcgetattr(fd, saved_opts) < 0) abort();
  /* Make a copy of saved_opts, not an alias. */
  new_opts = *saved_opts;
  new_opts.c_lflag    &= ~(ECHO | ICANON | IEXTEN | ISIG);
  new_opts.c_iflag    &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  new_opts.c_cflag    &= ~(CSIZE | PARENB);
  new_opts.c_cflag    |= CS8;
  new_opts.c_oflag    &= ~OPOST;
  new_opts.c_cc[VMIN]  = 1;
  new_opts.c_cc[VTIME] = 0;
  if (tcsetattr(fd, TCSAFLUSH, &new_opts) < 0) abort();
}

/* Query the terminal file descriptor with the TIOCGWINSZ ioctl in order to find
 * out the width of the terminal.  If we get an error, go ahead and assume a 78
 * character display.  The worst that may happen is bad wrapping.
 */
static int spc_get_barsize(int ttyfd) {
  struct winsize sz;

  if (ioctl(ttyfd, TIOCGWINSZ, (char *)&sz) < 0) return DEFAULT_BARSIZE;
  if (sz.ws_col < OVERHEAD_CHARS) return 0;
  if (sz.ws_col - OVERHEAD_CHARS > MAX_BARSIZE) return MAX_BARSIZE;
  return sz.ws_col - OVERHEAD_CHARS;
}

static void spc_show_progress_bar(double entropy, int target, int ttyfd) {
  int  bsz, c;
  char bf[MAX_BARSIZE + OVERHEAD_CHARS];

  bsz = spc_get_barsize(ttyfd);
  c   = (int)((entropy * bsz) / target);
  bf[sizeof(bf) - 1] = 0;
  if (bsz) {
    snprintf(bf, sizeof(bf), "\r[%-*s] %d%%", bsz, "",
             (int)(entropy * 100.0 / target));
    memset(bf + 2, '=', c);
    bf[c + 2] = '>';
  } else
    snprintf(bf, sizeof(bf), "\r%d%%", (int)(entropy * 100.0 / target));
  while (write(ttyfd, bf, strlen(bf)) == -1)
    if (errno != EAGAIN) abort();
}

static void spc_end_progress_bar(int target, int ttyfd) {
  int bsz, i;

  if (!(bsz = spc_get_barsize(ttyfd))) {
    printf("100%%\r\n");
    return;
  }
  printf("\r[");
  for (i = 0;  i < bsz;  i++) putchar('=');
  printf("] 100%%\r\n");
}

void spc_gather_keyboard_entropy(int l, char *output) {
  int            fd, n;
  char           lastc = 0;
  double         entropy = 0.0;
  SHA_CTX        pool;
  volatile char  dgst[HASH_OUT_SZ];
  struct termios opts;
  struct {
    char         c;
    long long    timestamp;
  }              data;

  if (l > HASH_OUT_SZ) abort();
  if ((fd = open("/dev/tty", O_RDWR)) == -1) abort();
  spc_raw(fd, &opts);
  SHA1_Init(&pool);
  do {
    spc_show_progress_bar(entropy, l * 8, fd);
    if ((n = read(fd, &(data.c), 1)) < 1) {
      if (errno == EAGAIN) continue;
      abort();
    }
    current_stamp(&(data.timestamp));
    SHA1_Update(&pool, &data, sizeof(data));
    if (lastc != data.c) entropy += ENTROPY_PER_SAMPLE;
    lastc = data.c;
  } while (entropy < (l * 8));
  spc_end_progress_bar(l * 8, fd);
  /* Try to reset the terminal. */
  tcsetattr(fd, TCSAFLUSH, &opts);
  close(fd);
  SHA1_Final((unsigned char *)dgst, &pool);
  memcpy(output, (char *)dgst, l);
  spc_memset(dgst, 0, sizeof(dgst));
}
