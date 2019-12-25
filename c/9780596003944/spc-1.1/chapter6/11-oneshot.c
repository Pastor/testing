int SPC_OMAC1(unsigned char key[], int keylen, unsigned char in[], size_t l,
              unsigned char out[16]) {
  SPC_OMAC_CTX c;

  if (!spc_omac1_init(&c, key, keylen)) return 0;
  spc_omac_update(&c, in, l);
  spc_omac_final(&c, out);
  return 1;
}

int SPC_OMAC2(unsigned char key[], int keylen, unsigned char in[], size_t l,
              unsigned char out[16]) {
  SPC_OMAC_CTX c;

  if (!spc_omac2_init(&c, key, keylen)) return 0;
  spc_omac_update(&c, in, l);
  spc_omac_final(&c, out);
  return 1;
}
