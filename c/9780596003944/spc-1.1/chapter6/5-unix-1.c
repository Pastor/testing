#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

int main(int argc, char *argv[]) {
  int           i;
  SHA_CTX       ctx;
  unsigned char result[SHA_DIGEST_LENGTH]; /* SHA1 has a 20-byte digest. */
  unsigned char *s1 = "Testing";
  unsigned char *s2 = "...1...2...3...";

  SHA1_Init(&ctx);
  SHA1_Update(&ctx, s1, strlen(s1));
  SHA1_Update(&ctx, s2, strlen(s2));
  /* Yes, the context object is last. */
  SHA1_Final(result, &ctx);

  printf("SHA1(\"%s%s\") = ", s1, s2);
  for (i = 0;  i < SHA_DIGEST_LENGTH;  i++) printf("%02x", result[i]);
  printf("\n");

  return 0;
}
