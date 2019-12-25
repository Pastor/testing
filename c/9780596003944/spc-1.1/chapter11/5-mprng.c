/* NOTE: This code should be augmented to reseed after each request
 *  for pseudo-random data, as discussed in Recipe 11.6.
 */
#ifndef WIN32
#include <string.h>
#include <pthread.h>
#else
#include <windows.h>
#endif

/* If MAC operations fail, you passed in a bad key size you are using a hardware
 * API that failed.  In that case, be sure to perform error checking.
 */
#define MAC_OUT_SZ 20

typedef struct {
  SPC_HMAC_CTX  ctx;
  unsigned char ctr[MAC_OUT_SZ];
  unsigned char lo[MAC_OUT_SZ];   /* Leftover block of output */
  int           ix;               /* index into lo. */
} SPC_MPRNG_CTX;

#ifndef WIN32
static pthread_mutex_t spc_mprng_mutex = PTHREAD_MUTEX_INITIALIZER;

#define SPC_MPRNG_LOCK() pthread_mutex_lock(&spc_mprng_mutex)
#define SPC_MPRNG_UNLOCK() pthread_mutex_unlock(&spc_mprng_mutex)
#else
static HANDLE hSpcMPRNGMutex;

#define SPC_MPRNG_LOCK() WaitForSingleObject(hSpcMPRNGMutex, INFINITE)
#define SPC_MPRNG_UNLOCK() ReleaseMutex(hSpcMPRNGMutex)
#endif

static void spc_increment_mcounter(SPC_MPRNG_CTX *prng) {
  int i = MAC_OUT_SZ;

  while (i--)
    if (++prng->ctr[i])
      return;
}

void spc_mprng_init(SPC_MPRNG_CTX *prng, unsigned char *seed, int l) {
  SPC_MPRNG_LOCK();
  SPC_HMAC_Init(&(prng->ctx), seed, l);
  memset(prng->ctr, 0, MAC_OUT_SZ);
  prng->ix = 0;
  SPC_MPRNG_UNLOCK();
}

unsigned char *spc_mprng_rand(SPC_MPRNG_CTX *prng, unsigned char *buf, size_t l) {
  unsigned char *p;

  SPC_MPRNG_LOCK();
  for (p = buf;  prng->ix && l;  l--) {
    *p++ = prng->lo[prng->ix++];
    prng->ix %= MAC_OUT_SZ;
  }
  while (l >= MAC_OUT_SZ) {
    SPC_HMAC_Reset(&(prng->ctx));
    SPC_HMAC_Update(&(prng->ctx), prng->ctr, sizeof(prng->ctr));
    SPC_HMAC_Final(p, &(prng->ctx));
    spc_increment_mcounter(prng);
    p += MAC_OUT_SZ;
    l -= MAC_OUT_SZ;
  }
  if (l) {
    SPC_HMAC_Reset(&(prng->ctx));
    SPC_HMAC_Update(&(prng->ctx), prng->ctr, sizeof(prng->ctr));
    SPC_HMAC_Final(prng->lo, &(prng->ctx));
    spc_increment_mcounter(prng);
    prng->ix = l;
    while (l--) p[l] = prng->lo[l];
  }
  SPC_MPRNG_UNLOCK();
  return buf;
}
