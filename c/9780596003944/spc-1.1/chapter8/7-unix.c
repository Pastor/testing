#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <paths.h>

#define BUF_STEP 1024 /* Allocate this much space for the password, and if it gets
                       * this long, reallocate twice the space.
                       * Rinse, lather, repeat.
                       */

static unsigned char *read_password(int termfd) {
  unsigned char ch, *ret, *tmp;
  unsigned long ctr = 0;

  if (!(ret = (unsigned char *)malloc(BUF_STEP + 1))) return 0;
  for (;;) {
    switch (read(termfd, &ch, 1)) {
      case 1:
        if (ch != '\n') break;
        /* FALL THROUGH */
      case 0:
        ret[ctr] = 0;
        return ret;
      default:
        free(ret);
        return 0;
    }
    ret[ctr] = ch;
    if (ctr && !(ctr & BUF_STEP)) {
      if (!(tmp = (unsigned char *)realloc(ret, ctr + BUF_STEP + 1))) {
        free(ret);
        return 0;
      }
      ret = tmp;
    }
    ctr++;
  }
}

unsigned char *spc_read_password(unsigned char *prompt, FILE *term) {
  int            close = 0, termfd;
  sigset_t       saved_signals, set_signals;
  unsigned char  *retval;
  struct termios saved_term, set_term;

  if (!term) {
    if (!(term = fopen(_PATH_TTY, "r+"))) return 0;
    close = 1;
  }

  termfd = fileno(term);
  fprintf(term, "%s", prompt);
  fflush(term);

  /* Defer interruption when echo is turned off */
  sigemptyset(&set_signals);
  sigaddset(&set_signals, SIGINT);
  sigaddset(&set_signals, SIGTSTP);
  sigprocmask(SIG_BLOCK, &set_signals, &saved_signals);

  /*Save the current state and set the terminal to not echo */
  tcgetattr(termfd, &saved_term);
  set_term = saved_term;
  set_term.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHONL);
  tcsetattr(termfd, TCSAFLUSH, &set_term);

  retval = read_password(termfd);
  fprintf(term, "\n");

  tcsetattr(termfd, TCSAFLUSH, &saved_term);
  sigprocmask(SIG_SETMASK, &saved_signals, 0);
  if (close) fclose(term);

  return retval;
}
