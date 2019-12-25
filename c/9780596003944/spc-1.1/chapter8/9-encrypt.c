#include <string.h>
#include <unistd.h>

char *spc_crypt_encrypt(const char *password) {
  char salt[3];
  static char *choices = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
                         "0123456789./";

  salt[0] = choices[spc_rand_range(0, strlen(choices) - 1)];
  salt[1] = choices[spc_rand_range(0, strlen(choices) - 1)];
  salt[2] = 0;
  return crypt(password, salt);
}
