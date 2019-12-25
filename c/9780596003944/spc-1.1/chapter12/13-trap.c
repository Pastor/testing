#include <stdio.h>
#include <signal.h>

#define SPC_DEBUGGER_PRESENT (num_traps == 0)
static int num_traps = 0;

static void dbg_trap(int signo) {
  num_traps++;
}

int spc_trap_detect(void) {
  if (signal(SIGTRAP, dbg_trap) == SIG_ERR) return 0;
  raise(SIGTRAP);
  return 1;
}
