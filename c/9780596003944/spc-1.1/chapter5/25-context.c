#include <windows.h>
#include <wincrypt.h>

HCRYPTPROV SpcGetCryptContext(void) {
  HCRYPTPROV hProvider;

  if (!CryptAcquireContext(&hProvider, 0, MS_ENHANCED_PROV, PROV_RSA_FULL,
                           CRYPT_VERIFYCONTEXT)) return 0;
  return hProvider;
}
