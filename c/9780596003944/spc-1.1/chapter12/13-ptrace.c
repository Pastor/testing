#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

int spc_detect_ptrace(void) {
  int   status, waitrc;
  pid_t child, parent;

  parent = getpid();
  if (!(child = fork())) {
    /* this is the child process */
    if (ptrace(PT_ATTACH, parent, 0, 0)) exit(1);
    do {
      waitrc = waitpid(parent, &status, 0);
    } while (waitrc == -1 && errno == EINTR);
    ptrace(PT_DETACH, parent, (caddr_t)1, SIGCONT);
    exit(0);
  }

  if (child == -1) return -1;

  do {
    waitrc = waitpid(child, &status, 0);
  } while (waitrc == -1 && errno == EINTR);

  return WEXITSTATUS(status);
}
