#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

/* Here, l is the output length of spc_create_nonced_digest() */
#define spc_extract_nonce(l, s)  (s)
#define spc_extract_digest(l, s) ((s) + ((l) / 2))

int main(int argc, char *argv[]) {
  unsigned int  i, ol;
  unsigned char *s = "Testing hashes with nonces.";
  unsigned char *dgst, *nonce, *ret;

  ret   = spc_create_nonced_digest(EVP_sha1(), s, strlen(s), &ol);
  nonce = spc_extract_nonce(ol, ret);
  dgst  = spc_extract_digest(ol, ret);
  printf("Nonce = ");
  for (i = 0;  i < ol / 2;  i++)
    printf("%02x", nonce[i]);
  printf("\nSHA1-Nonced(Nonce, \"%s\") = \n\t", s);
  for (i = 0;  i < ol / 2;  i++)
    printf("%02x", dgst[i]);
  printf("\n");
  if (spc_verify_nonced_digest(EVP_sha1(), s, strlen(s), ret))
    printf("Recalculation verified integrity.\n");
  else
    printf("Recalculation FAILED to match.\n");
  return 0;
}
