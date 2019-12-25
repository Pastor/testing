void my_func(void) {
  int x;

  for (x = 0; x < 10; x++) printf("%x\n", x);
  DISASM_FALSERET;
  /* other stuff can be done here that won't be disassembled */
}
