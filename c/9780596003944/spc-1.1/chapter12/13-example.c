int main(int argc, char *argv[]) {
  int x;

  spc_trap_detect();
  for (x = 0; x < 10; x++) {
    if (SPC_DEBUGGER_PRESENT) printf("being debugged!\n");
    else printf("y\n");
  }
  return(0);
}
