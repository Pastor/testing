void spc_add_padding(unsigned char *pad_goes_here, int ptlen, int bl) {
   int i, n = (ptlen - 1) % bl + 1;

   for (i = 0;  i < n;  i++) *(pad_goes_here + i) = (unsigned char)n;
}

int spc_remove_padding(unsigned char *lastblock, int bl) {
  unsigned char i, n = lastblock[bl - 1];
  unsigned char *p = lastblock + bl;

   /* In your programs you should probably throw an exception or abort instead. */
  if (n > bl || n <= 0) return -1;
  for (i = n;  i;  i--) if (*--p != n) return -1;
  return bl - n;
}
