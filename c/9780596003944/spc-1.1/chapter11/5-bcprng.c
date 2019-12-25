/* NOTE: This code should be augmented to reseed after each request
 *  for pseudo-random data, as discussed in Recipe 11.6.
 */
#ifndef WIN32
#include <string.h>
#include <pthread.h>
#else
#include <windows.h>
#endif

/* if encryption operations fail, you passed in a bad key size or are using a
 * hardware API that failed.  In that case, be sure to perform error checking.
*/

typedef struct {
  SPC_KEY_SCHED ks;
  unsigned char ctr[SPC_BLOCK_SZ];
  unsigned char lo[SPC_BLOCK_SZ]; /* Leftover block of output */
  int           ix;               /* index into lo */
  int           kl;               /* The length of key used to key the cipher */
} SPC_BCPRNG_CTX;

#ifndef WIN32
static pthread_mutex_t spc_bcprng_mutex = PTHREAD_MUTEX_INITIALIZER;

#define SPC_BCPRNG_LOCK() pthread_mutex_lock(&spc_bcprng_mutex);
#define SPC_BCPRNG_UNLOCK() pthread_mutex_unlock(&spc_bcprng_mutex);
#else
static HANDLE hSpcBCPRNGMutex;

#define SPC_BCPRNG_LOCK() WaitForSingleObject(hSpcBCPRNGMutex, INFINITE)
#define SPC_BCPRNG_UNLOCK() ReleaseMutex(hSpcBCPRNGMutex)
#endif

static void spc_increment_counter(SPC_BCPRNG_CTX *prng) {
  int i = SPC_BLOCK_SZ;

  while (i--)
    if (++prng->ctr[i]) return;
}

void spc_bcprng_init(SPC_BCPRNG_CTX *prng, unsigned char *key, int kl,
                     unsigned char *x, int xl) {
  int i = 0;

  SPC_BCPRNG_LOCK();
  SPC_ENCRYPT_INIT(&(prng->ks), key, kl);
  memset(prng->ctr, 0, SPC_BLOCK_SZ);
  while (xl-- && i < SPC_BLOCK_SZ)
    prng->ctr[i++] = *x++;
  prng->ix = 0;
  prng->kl = kl;
  SPC_BCPRNG_UNLOCK();
}

unsigned char *spc_bcprng_rand(SPC_BCPRNG_CTX *prng, unsigned char *buf, size_t l) {
  unsigned char *p;

  SPC_BCPRNG_LOCK();
  for (p = buf;  prng->ix && l;  l--) {
    *p++ = prng->lo[prng->ix++];
    prng->ix %= SPC_BLOCK_SZ;
  }
  while (l >= SPC_BLOCK_SZ) {
    SPC_DO_ENCRYPT(&(prng->ks), prng->ctr, p);
    spc_increment_counter(prng);
    p += SPC_BLOCK_SZ;
    l -= SPC_BLOCK_SZ;
  }
  if (l) {
    SPC_DO_ENCRYPT(&(prng->ks), prng->ctr, prng->lo);
    spc_increment_counter(prng);
    prng->ix = l;
    while (l--) p[l] = prng->lo[l];
  }
  SPC_BCPRNG_UNLOCK();
  return buf;
}
