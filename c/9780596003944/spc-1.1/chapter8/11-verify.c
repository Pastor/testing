int spc_pbkdf2_verify(const char *plain_password, const char *crypt_password) {
  int  match = 0;
  char *pbkdf2_result;

  if ((pbkdf2_result = spc_pbkdf2_encrypt(plain_password, crypt_password)) != 0) {
    match = !strcmp(pbkdf2_result, crypt_password);
    free(pbkdf2_result);
  }
  return match;
}
