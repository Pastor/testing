#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

int main(int argc, char *argv[]) {
  int           i, ol;
  EVP_MD_CTX    ctx;
  unsigned char *result;
  unsigned char *s1 = "Testing";
  unsigned char *s2 = "...1...2...3...";

  EVP_DigestInit(&ctx, EVP_sha1());
  EVP_DigestUpdate(&ctx, s1, strlen(s1));
  EVP_DigestUpdate(&ctx, s2, strlen(s2));
  if (!(result = (unsigned char *)malloc(EVP_MD_CTX_block_size(&ctx)))) abort();
  EVP_DigestFinal(&ctx, result, &ol);

  printf("SHA1(\"%s%s\") = ", s1, s2);
  for (i = 0;  i < ol;  i++) printf("%02x", result[i]);
  printf("\n");

  free(result);
  return 0;
}
