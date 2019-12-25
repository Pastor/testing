#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

char *spc_pbkdf2_encrypt(const char *key, const char *salt) {
  int           error;
  char          *base64_out, *base64_salt, *result, *salt_end, *tmp_string;
  size_t        length, result_length, salt_length;
  unsigned int  iterations, tmp_uint;
  unsigned char out[16], *raw_salt;
  unsigned long tmp_ulong;

  raw_salt = 0;
  base64_out = base64_salt = result = 0;

  if (!salt) {
    if (!(raw_salt = (unsigned char *)malloc((salt_length = 8)))) return 0;
    spc_rand(raw_salt, salt_length);
    if (!(base64_salt = spc_base64_encode(raw_salt, salt_length, 0))) {
      free(raw_salt);
      return 0;
    }
    iterations = 10000;
  } else {
    if (strncmp(salt, "$10$", 4) != 0) return 0;
    if (!(salt_end = strchr(salt + 4, '$'))) return 0;
    if (!(base64_salt = (char *)malloc(salt_end - (salt + 4) + 1))) return 0;
    memcpy(base64_salt, salt + 4, salt_end - (salt + 4));
    base64_salt[salt_end - (salt + 4)] = 0;
    tmp_ulong = strtoul(salt_end + 1, &tmp_string, 10);
    if ((tmp_ulong == ULONG_MAX && errno == ERANGE) || tmp_ulong > UINT_MAX ||
        !tmp_string || *tmp_string != '$') {
      free(base64_salt);
      return 0;
    }
    iterations = (unsigned int)tmp_ulong;
    raw_salt = spc_base64_decode(base64_salt, &salt_length, 1, &error);
    if (!raw_salt || error) {
      free(base64_salt);
      return 0;
    }
  }

  spc_pbkdf2((char *)key, strlen(key), raw_salt, salt_length, iterations,
             out, sizeof(out));

  if (!(base64_out = spc_base64_encode(out, sizeof(out), 0))) goto done;
  for (tmp_uint = iterations, length = 1;  tmp_uint; length++) tmp_uint /= 10;
  result_length = strlen(base64_out) + strlen(base64_salt) + length + 6;
  if (!(result = (char *)malloc(result_length + 1))) goto done;
  sprintf(result, "$10$%s$%u$%s",  base64_salt, iterations, base64_out);

done:
  /* cleanup */
  if (raw_salt) free(raw_salt);
  if (base64_salt) free(base64_salt);
  if (base64_out) free(base64_out);
  return result;
}
