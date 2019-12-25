#include <stdio.h>
#include <openssl/rc4.h>
#include <openssl/sha.h>

#define HASH_SZ   20
#define NUM_WORDS (HASH_SZ / sizeof(int))

void spc_lion_encrypt(char *in, char *out, size_t blklen, char *key) {
  int     i, tmp[NUM_WORDS];
  RC4_KEY k;

  /* Round 1: R = R ^ RC4(L ^ K1) */
  for (i = 0;  i < NUM_WORDS;  i++)
    tmp[i] = ((int *)in)[i] ^ ((int *)key)[i];
  RC4_set_key(&k, HASH_SZ, (char *)tmp);
  RC4(&k, blklen - HASH_SZ, in + HASH_SZ, out + HASH_SZ);

  /* Round 2: L = L ^ SHA1(R) */
  SHA1(out + HASH_SZ, blklen - HASH_SZ, out);
  for (i = 0;  i < NUM_WORDS; i++)
    ((int *)out)[i] ^= ((int *)in)[i];

  /* Round 3: R = R ^ RC4(L ^ K2) */
  for (i = 0;  i < NUM_WORDS;  i++)
    tmp[i] = ((int *)out)[i] ^ ((int *)key)[i + NUM_WORDS];
  RC4_set_key(&k, HASH_SZ, (char *)tmp);
  RC4(&k, blklen - HASH_SZ, out + HASH_SZ, out + HASH_SZ);
}

void spc_lion_decrypt(char *in, char *out, size_t blklen, char *key) {
  int     i, tmp[NUM_WORDS];
  RC4_KEY k;

  for (i = 0;  i < NUM_WORDS;  i++)
    tmp[i] = ((int *)in)[i] ^ ((int *)key)[i + NUM_WORDS];
  RC4_set_key(&k, HASH_SZ, (char *)tmp);
  RC4(&k, blklen - HASH_SZ, in + HASH_SZ, out + HASH_SZ);

  SHA1(out + HASH_SZ, blklen - HASH_SZ, out);
  for (i = 0;  i < NUM_WORDS;  i++) {
    ((int *)out)[i] ^= ((int *)in)[i];
    tmp[i] = ((int *)out)[i] ^ ((int *)key)[i];
  }
  RC4_set_key(&k, HASH_SZ, (char *)tmp);
  RC4(&k, blklen - HASH_SZ, out + HASH_SZ, out + HASH_SZ);
}
