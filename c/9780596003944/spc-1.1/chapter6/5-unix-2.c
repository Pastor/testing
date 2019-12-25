#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

int main(int argc, char *argv[]) {
  int           i, ol;
  EVP_MD_CTX    ctx;
  unsigned char result[EVP_MAX_MD_SIZE]; /* enough for any hash function */
  unsigned char *s1 = "Testing";
  unsigned char *s2 = "...1...2...3...";

  /* Note the extra parameter */
  EVP_DigestInit(&ctx, EVP_sha1());
  EVP_DigestUpdate(&ctx, s1, strlen(s1));
  EVP_DigestUpdate(&ctx, s2, strlen(s2));
  /* Here, the context object is first. Notice the pointer to the output length */
  EVP_DigestFinal(&ctx, result, &ol);

  printf("SHA1(\"%s%s\") = ", s1, s2);
  for (i = 0;  i < ol;  i++) printf("%02x", result[i]);
  printf("\n");

  return 0;
}
