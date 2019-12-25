int spc_cfb_final(SPC_CFB_CTX *ctx) {
  spc_memset(&ctx, 0, sizeof(SPC_CFB_CTX));
  return 1;
}
