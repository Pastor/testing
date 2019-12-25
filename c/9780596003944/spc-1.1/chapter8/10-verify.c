int spc_md5_verify(const char *plain_password, const char *crypt_password) {
  int  match = 0;
  char *md5_result;

  if ((md5_result = spc_md5_encrypt(plain_password, crypt_password)) != 0) {
    match = !strcmp(md5_result, crypt_password);
    free(md5_result);
  }
  return match;
}
