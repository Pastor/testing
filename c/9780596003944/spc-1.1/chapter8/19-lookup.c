#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int get_keydata(EVP_PKEY *key, char **keydata) {
  BIO   *b64 = 0, *bio = 0;
  int   keytype, length;
  char  *dummy;

  *keydata = 0;
  keytype = EVP_PKEY_type(key->type);
  if (!(length = i2d_PublicKey(key, 0))) goto error_exit;
  if (!(dummy = *keydata = (char *)malloc(length))) goto error_exit;
  i2d_PublicKey(key, (unsigned char **)&dummy);

  if (!(bio = BIO_new(BIO_s_mem()))) goto error_exit;
  if (!(b64 = BIO_new(BIO_f_base64()))) goto error_exit;
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
  if (!(bio = BIO_push(b64, bio))) goto error_exit;
  b64 = 0;
  BIO_write(bio, *keydata, length);

  free(*keydata);  *keydata = 0;
  if (!(length = BIO_get_mem_data(bio, &dummy))) goto error_exit;
  if (!(*keydata = (char *)malloc(length + 1))) goto error_exit;
  memcpy(*keydata, dummy, length);
  (*keydata)[length - 1] = '\0';
  return keytype;

error_exit:
  if (b64) BIO_free_all(b64);
  if (bio) BIO_free_all(bio);
  if (*keydata) free(*keydata);
  *keydata = 0;
  return EVP_PKEY_NONE;
}

static int parse_line(char *line, char **ipnum, int *keytype, char **keydata) {
  char  *end, *p, *tmp;

  /* we expect leading and trailing whitespace to be stripped already */
  for (p = line;  *p && !isspace(*p);  p++);
  if (!*p) return 0;
  *ipnum = line;

  for (*p++ = '\0';  *p && isspace(*p);  p++);
  for (tmp = p;  *p && !isspace(*p);  p++);
  *keytype = (int)strtol(tmp, &end, 0);
  if (*end && !isspace(*end)) return 0;

  for (p = end;  *p && isspace(*p);  p++);
  for (tmp = p;  *p && !isspace(*p);  p++);
  if (*p) return 0;
  *keydata = tmp;

  return 1;
}

int spc_lookup_key(char *filename, char *ipnum, EVP_PKEY *key) {
  int   bufsize = 0, length, keytype, lineno = 0, result = 0, store_keytype;
  char  *buffer = 0, *keydata, *line, *store_ipnum, *store_keydata, tmp[1024];
  FILE  *fp = 0;

  keytype = get_keydata(key, &keydata);
  if (keytype == EVP_PKEY_NONE || !keydata) goto end;

  if (!(fp = fopen(filename, "r"))) goto end;
  while (fgets(tmp, sizeof(tmp), fp)) {
    length = strlen(tmp);
    buffer = (char *)realloc(buffer, bufsize + length + 1);
    memcpy(buffer + bufsize, tmp, length + 1);
    bufsize += length;
    if (buffer[bufsize - 1] != '\n') continue;
    while (bufsize && (buffer[bufsize - 1] == '\r' || buffer[bufsize - 1] == '\n'))
      bufsize--;
    buffer[bufsize] = '\0';
    bufsize = 0;
    lineno++;

    for (line = buffer;  isspace(*line);  line++);
    for (length = strlen(line);  length && isspace(line[length - 1]);  length--);
    line[length - 1] = '\0';
    /* blank lines and lines beginning with # or ; are ignored */
    if (!length || line[0] == '#' || line[0] == ';') continue;
    if (!parse_line(line, &store_ipnum, &store_keytype, &store_keydata)) {
      fprintf(stderr, "%s:%d: parse error\n", filename, lineno);
      continue;
    }
    if (inet_addr(store_ipnum) != inet_addr(ipnum)) continue;
    if (store_keytype != keytype || strcasecmp(store_keydata, keydata))
      result = -1;
    else result = 1;
    break;
  }

end:
  if (buffer) free(buffer);
  if (keydata) free(keydata);
  if (fp) fclose(fp);
  return result;
}
