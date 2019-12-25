#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

int main(int argc, char *argv[]) {
  int           i;
  unsigned int  ol;
  unsigned char *s = "Testing...1...2...3...";
  unsigned char *r;

  r = spc_digest_message(EVP_sha1(), s, strlen(s), &ol);

  printf("SHA1(\"%s\") = ", s);
  for (i = 0;  i < ol;  i++) printf("%02x", r[i]);
  printf("\n");

  free(r);
  return 0;
}
