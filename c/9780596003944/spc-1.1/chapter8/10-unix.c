#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

static char *crypt64_encode(const unsigned char *buf) {
  int           i;
  char          *out, *ptr;
  unsigned long l;

  static char   *crypt64_set = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz";

  if (!(out = ptr = (char *)malloc(23))) return 0;

#define CRYPT64_ENCODE(x, y, z) \
  for (i = 0, l = (buf[(x)] << 16) | (buf[(y)] << 8) | buf[(z)];  i++ < 4; \
    l >>= 6) *ptr++ = crypt64_set[l & 0x3F]

  CRYPT64_ENCODE(0,  6, 12);  CRYPT64_ENCODE(1,  7, 13);
  CRYPT64_ENCODE(2,  8, 14);  CRYPT64_ENCODE(3,  9, 15);
  CRYPT64_ENCODE(4, 10,  5);

  for (i = 0, l = buf[11];  i++ < 2;  l >>= 6) *ptr++ = crypt64_set[l & 0x3F];
  *ptr = 0;

#undef CRYPT64_ENCODE

  return out;
}

static void compute_hash(unsigned char *hash, const char *key,
                         const char *salt, size_t salt_length) {
  int     i, length;
  size_t  key_length;
  MD5_CTX ctx, ctx1;

  key_length = strlen(key);
  MD5_Init(&ctx);
  MD5_Update(&ctx, key, key_length);
  MD5_Update(&ctx, salt, salt_length);

  MD5_Init(&ctx1);
  MD5_Update(&ctx1, key, key_length);
  MD5_Update(&ctx1, salt, salt_length);
  MD5_Update(&ctx1, key, key_length);
  MD5_Final(hash, &ctx1);

  for (length = key_length;  length > 0;  length -= 16)
    MD5_Update(&ctx, hash, (length > 16 ? 16 : length));
  memset(hash, 0, 16);
  for (i = key_length;  i;  i >>= 1)
    if (i & 1) MD5_Update(&ctx, hash, 1);
    else MD5_Update(&ctx, key, 1);
  MD5_Final(hash, &ctx);

  for (i = 0;  i < 1000;  i++) {
    MD5_Init(&ctx);
    if (i & 1) MD5_Update(&ctx, key, key_length);
    else MD5_Update(&ctx, hash, 16);
    if (i % 3) MD5_Update(&ctx, salt, salt_length);
    if (i % 7) MD5_Update(&ctx, key, key_length);
    if (i & 1) MD5_Update(&ctx, hash, 16);
    else MD5_Update(&ctx, key, key_length);
    MD5_Final(hash, &ctx);
  }
}

char *spc_md5_encrypt(const char *key, const char *salt) {
  char          *base64_out, *base64_salt, *result, *salt_end, *tmp_string;
  size_t        result_length, salt_length;
  unsigned char out[16], raw_salt[16];

  base64_out = base64_salt = result = 0;

  if (!salt) {
    salt_length = 8;
    spc_rand(raw_salt, sizeof(raw_salt));
    if (!(base64_salt = crypt64_encode(raw_salt))) goto done;
    if (!(tmp_string = (char *)realloc(base64_salt, salt_length + 1)))
      goto done;
    base64_salt = tmp_string;
  } else {
    if (strncmp(salt, "$1$", 3) != 0) goto done;
    if (!(salt_end = strchr(salt + 3, '$'))) goto done;
    salt_length = salt_end - (salt + 3);
    if (salt_length > 8) salt_length = 8; /* maximum salt is 8 bytes */
    if (!(base64_salt = (char *)malloc(salt_length + 1))) goto done;
    memcpy(base64_salt, salt + 3, salt_length);
  }
  base64_salt[salt_length] = 0;

  compute_hash(out, key, base64_salt, salt_length);

  if (!(base64_out = crypt64_encode(out))) goto done;
  result_length = strlen(base64_out) + strlen(base64_salt) + 5;
  if (!(result = (char *)malloc(result_length + 1))) goto done;
  sprintf(result, "$1$%s$%s", base64_salt, base64_out);

done:
  /* cleanup */
  if (base64_salt) free(base64_salt);
  if (base64_out) free(base64_out);
  return result;
}
