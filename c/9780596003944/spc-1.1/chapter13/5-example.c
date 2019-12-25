#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int sigint_received = 0;
int sigterm_received = 0;
int sigquit_received = 0;

void handle_sigint(int sig)  { sigint_received = 1;  }
void handle_sigterm(int sig) { sigterm_received = 1; }
void handle_sigquit(int sig) { sigquit_received = 1; }

static void setup_signal_handler(int sig, void (*handler)( )) {
#if _POSIX_VERSION > 198800L
  struct sigaction action;

  action.sa_handler = handler;
  sigemptyset(&(action.sa_mask));
  sigaddset(&(action.sa_mask), sig);
  action.sa_flags = 0;
  sigaction(sig, &action, 0);
#else
  signal(sig, handler);
#endif
}

static int signal_was_caught(void)
{
  if (sigint_received) printf("SIGINT received!\n");
  if (sigterm_received) printf("SIGTERM received!\n");
  if (sigquit_received) printf("SIGQUIT received!\n");
  return (sigint_received || sigterm_received || sigquit_received);
}

int main(int argc, char *argv[]) {
  char buffer[80];

  setup_signal_handler(SIGINT, handle_sigint);
  setup_signal_handler(SIGTERM, handle_sigterm);
  setup_signal_handler(SIGQUIT, handle_sigquit);

  /* The main loop of this program simply reads input from stdin, and
   * throws it away.  It's useless functionality, but the point is to
   * illustrate signal handling, and fread is a system call that will
   * be interrupted by signals, so it works well for example purposes
   */
  while (!feof(stdin)) {
    fread(buffer, 1, sizeof(buffer), stdin);
    if (signal_was_caught()) break;
  }

  return (sigint_received || sigterm_received || sigquit_received);
}
