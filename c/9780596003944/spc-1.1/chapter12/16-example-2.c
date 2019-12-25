void my_func(void) {
  int x;

  DISASM_MISALIGN;
  for (x = 0;  x < 10;  x++) printf("%x\n", x);
}
