void spc_OMAC1_nonced(unsigned char key[], int keylen, unsigned char in[],
                      size_t l, unsigned char nonce[16], unsigned char out[16]) {
  SPC_OMAC_CTX c;

  if (!spc_omac1_init(&c, key, keylen)) abort();
  spc_omac_update(&c, nonce, 16);
  spc_omac_update(&c, in, l);
  spc_omac_final(&c, out);
}
