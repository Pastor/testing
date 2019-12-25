#include <windows.h>
#include <wincrypt.h>

static HCRYPTPROV hProvider;

void spc_rand_init(void) {
  if (!CryptAcquireContext(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    ExitProcess((UINT)-1);  /* Feel free to properly signal an error instead. */
}

unsigned char *spc_rand(unsigned char *pbBuffer, size_t cbBuffer) {
  if (!hProvider) spc_rand_init();
  if (!CryptGenRandom(hProvider, cbBuffer, pbBuffer))
    ExitProcess((UINT)-1); /* Feel free to properly signal an error instead. */
  return pbBuffer;
}

unsigned char *spc_keygen(unsigned char *pbBuffer, size_t cbBuffer) {
  if (!hProvider) spc_rand_init();
  if (!CryptGenRandom(hProvider, cbBuffer, pbBuffer))
    ExitProcess((UINT)-1);
  return pbBuffer;
}
