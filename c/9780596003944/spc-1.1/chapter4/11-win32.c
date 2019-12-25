#include <windows.h>
#include <wincrypt.h>

#define HMAC_OUT_LEN 20 /* SHA1 specific */

static DWORD SwapInt32(DWORD dwInt32) {
  __asm mov   eax, dwInt32
  __asm bswap eax
}

BOOL SpcMakeDerivedKey(BYTE *pbBase, DWORD cbBase, BYTE *pbDist, DWORD cbDist,
                       BYTE *pbOut, DWORD cbOut) {
  BYTE       pbLast[HMAC_OUT_LEN];
  DWORD      cbData, dwCounter = 0, dwBigCounter;
  HCRYPTKEY  hKey;
  HMAC_INFO  HMACInfo;
  HCRYPTHASH hHash;
  HCRYPTPROV hProvider;

  if (!(hProvider = SpcGetExportableContext())) return FALSE;
  if (!(hKey = SpcImportKeyData(hProvider, CALG_RC4, pbBase, cbBase))) {
    CryptReleaseContext(hProvider, 0);
    return FALSE;
  }
  HMACInfo.HashAlgid     = CALG_SHA1;
  HMACInfo.pbInnerString = HMACInfo.pbOuterString = 0;
  HMACInfo.cbInnerString = HMACInfo.cbOuterString = 0;

  while (cbOut >= HMAC_OUT_LEN) {
    if (!CryptCreateHash(hProvider, CALG_HMAC, hKey, 0, &hHash)) {
      CryptDestroyKey(hKey);
      CryptReleaseContext(hProvider, 0);
      return FALSE;
    }
    CryptSetHashParam(hHash, HP_HMAC_INFO, (BYTE *)&HMACInfo, 0);
    CryptHashData(hHash, pbDist, cbDist, 0);
    dwBigCounter = SwapInt32(dwCounter++);
    CryptHashData(hHash, (BYTE *)&dwBigCounter, sizeof(dwBigCounter), 0);
    cbData = HMAC_OUT_LEN;
    CryptGetHashParam(hHash, HP_HASHVAL, pbOut, &cbData, 0);
    CryptDestroyHash(hHash);
    pbOut += HMAC_OUT_LEN;
    cbOut -= HMAC_OUT_LEN;
  }
  if (cbOut) {
    if (!CryptCreateHash(hProvider, CALG_HMAC, hKey, 0, &hHash)) {
      CryptDestroyKey(hKey);
      CryptReleaseContext(hProvider, 0);
      return FALSE;
    }
    CryptSetHashParam(hHash, HP_HMAC_INFO, (BYTE *)&HMACInfo, 0);
    CryptHashData(hHash, pbDist, cbDist, 0);
    dwBigCounter = SwapInt32(dwCounter);
    CryptHashData(hHash, (BYTE *)&dwBigCounter, sizeof(dwBigCounter), 0);
    cbData = HMAC_OUT_LEN;
    CryptGetHashParam(hHash, HP_HASHVAL, pbLast, &cbData, 0);
    CryptDestroyHash(hHash);
    CopyMemory(pbOut, pbLast, cbOut);
  }

  CryptDestroyKey(hKey);
  CryptReleaseContext(hProvider, 0);
  return TRUE;
}
