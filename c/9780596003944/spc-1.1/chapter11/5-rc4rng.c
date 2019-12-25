/* NOTE: This code should be augmented to reseed after each request
 *  for pseudo-random data, as discussed in Recipe 11.6.
 */
#ifndef WIN32
#include <pthread.h>

static pthread_mutex_t spc_rc4rng_mutex = PTHREAD_MUTEX_INITIALIZER;

#define SPC_RC4RNG_LOCK() pthread_mutex_lock(&spc_rc4rng_mutex)
#define SPC_RC4RNG_UNLOCK() pthread_mutex_unlock(&spc_rc4rng_mutex)
#else
#include <windows.h>

static HANDLE hSpcRC4RNGMutex;

#define SPC_RC4RNG_LOCK() WaitForSingleObject(hSpcRC4RNGMutex, INFINITE)
#define SPC_RC4RNG_UNLOCK() ReleaseMutex(hSpcRC4RNGMutex)
#endif

#define SPC_ARBITRARY_SIZE 16

unsigned char *spc_rand(unsigned char *buf, size_t l) {
  static unsigned char zeros[SPC_ARBITRARY_SIZE] = {0,};
  unsigned char       *p = buf;

#ifdef WIN32
  if (!hSpcRC4RNGMutex) hSpcRC4RNGMutex = CreateMutex(0, FALSE, 0);
#endif

  SPC_RC4RNG_LOCK();
  while (l >= SPC_ARBITRARY_SIZE) {
    RC4(&spc_prng, SPC_ARBITRARY_SIZE, zeros, p);
    l -= SPC_ARBITRARY_SIZE;
    p += SPC_ARBITRARY_SIZE;
  }
  if (l) RC4(&spc_prng, l, zeros, p);

  SPC_RC4RNG_UNLOCK();
  return buf;
}
