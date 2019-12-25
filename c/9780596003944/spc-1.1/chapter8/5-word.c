#include <string.h>

static char *spc_password_characters = "abcdefghijklmnopqrstuvwxyz0123456789"
 "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()"
                                       "-=_+;[]{ }\\|,./<>?;";

char *spc_generate_password(char *buf, size_t bufsz) {
  size_t choices, i;

  choices = strlen(spc_password_characters) - 1;
  for (i = 0;  i < bufsz - 1;  i++) /* leave room for NULL terminator */
    buf[i] = spc_password_characters[spc_rand_range(0, choices)];
  buf[bufsz - 1] = 0;
  return buf;
}
