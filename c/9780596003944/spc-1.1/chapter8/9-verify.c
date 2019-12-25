#include <string.h>
#include <unistd.h>

int spc_crypt_verify(const char *plain_password, const char *cipher_password) {
  return !strcmp(cipher_password, crypt(plain_password, cipher_password));
}
