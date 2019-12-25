#include <windows.h>
#include <wincrypt.h>

HCRYPTPROV SpcGetExportableContext(void) {
  HCRYPTPROV hProvider;

  if (!CryptAcquireContext(&hProvider, 0, MS_ENHANCED_PROV, PROV_RSA_FULL, 0)) {
    if (GetLastError() != NTE_BAD_KEYSET) return 0;
    if (!CryptAcquireContext(&hProvider, 0, MS_ENHANCED_PROV, PROV_RSA_FULL,
                            CRYPT_NEWKEYSET)) return 0;
  }
  return hProvider;
}
