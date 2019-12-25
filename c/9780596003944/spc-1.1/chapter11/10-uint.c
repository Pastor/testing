unsigned int spc_rand_uint(void) {
  unsigned int res;

  spc_rand((unsigned char *)&res, sizeof(unsigned int));
  return res;
}
