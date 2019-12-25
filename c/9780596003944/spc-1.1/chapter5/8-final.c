int spc_ofb_final(SPC_OFB_CTX *ctx) {
  spc_memset(&ctx, 0, sizeof(SPC_OFB_CTX));
  return 1;
}
