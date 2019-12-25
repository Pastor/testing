#include <windows.h>
#include <wincrypt.h>

/* This value needs to be the output size of your pseudo-random function (PRF)! */
#define PRF_OUT_LEN 20

/* This is an implementation of the PKCS#5 PBKDF2 PRF using HMAC-SHA1.  It
 * always gives 20-byte outputs.
 */

static HCRYPTHASH InitHMAC(HCRYPTPROV hProvider, HCRYPTKEY hKey, ALG_ID Algid) {
  HMAC_INFO  HMACInfo;
  HCRYPTHASH hHash;

  HMACInfo.HashAlgid     = Algid;
  HMACInfo.pbInnerString = HMACInfo.pbOuterString = 0;
  HMACInfo.cbInnerString = HMACInfo.cbOuterString = 0;

  if (!CryptCreateHash(hProvider, CALG_HMAC, hKey, 0, &hHash)) return 0;
  CryptSetHashParam(hHash, HP_HMAC_INFO, (BYTE *)&HMACInfo, 0);
  return hHash;
}

static void FinalHMAC(HCRYPTHASH hHash, BYTE *pbOut, DWORD *cbOut) {
  *cbOut = PRF_OUT_LEN;
  CryptGetHashParam(hHash, HP_HASHVAL, pbOut, cbOut, 0);
  CryptDestroyHash(hHash);
}

static DWORD SwapInt32(DWORD dwInt32) {
  __asm mov   eax, dwInt32
  __asm bswap eax
}

static BOOL PKCS5InitialPRF(HCRYPTPROV hProvider, HCRYPTKEY hKey,
                            BYTE *pbSalt, DWORD cbSalt, DWORD dwCounter,
                            BYTE *pbOut, DWORD *cbOut) {
  HCRYPTHASH hHash;

  if (!(hHash = InitHMAC(hProvider, hKey, CALG_SHA1))) return FALSE;
  CryptHashData(hHash, pbSalt, cbSalt, 0);
  dwCounter = SwapInt32(dwCounter);
  CryptHashData(hHash, (BYTE *)&dwCounter, sizeof(dwCounter), 0);
  FinalHMAC(hHash, pbOut, cbOut);
  return TRUE;
}

static BOOL PKCS5UpdatePRF(HCRYPTPROV hProvider, HCRYPTKEY hKey,
                           BYTE *pbSalt, DWORD cbSalt,
                           BYTE *pbOut, DWORD *cbOut) {
  HCRYPTHASH hHash;

  if (!(hHash = InitHMAC(hProvider, hKey, CALG_SHA1))) return FALSE;
  CryptHashData(hHash, pbSalt, cbSalt, 0);
  FinalHMAC(hHash, pbOut, cbOut);
  return TRUE;
}

static BOOL PKCS5FinalPRF(HCRYPTPROV hProvider, HCRYPTKEY hKey,
                          BYTE *pbSalt, DWORD cbSalt, DWORD dwIterations,
                          DWORD dwBlock, BYTE *pbOut) {
  BYTE  pbBuffer[PRF_OUT_LEN];
  DWORD cbBuffer, dwIndex, dwIteration = 1;

  SecureZeroMemory(pbOut, PRF_OUT_LEN);
  if (!(PKCS5InitialPRF(hProvider, hKey, pbSalt, cbSalt, dwBlock, pbBuffer,
                        &cbBuffer))) return FALSE;
  while (dwIteration < dwIterations) {
    for (dwIndex = 0;  dwIndex < PRF_OUT_LEN;  dwIndex++)
      pbOut[dwIndex] ^= pbBuffer[dwIndex];
    if (!(PKCS5UpdatePRF(hProvider, hKey, pbBuffer, PRF_OUT_LEN, pbBuffer,
                         &cbBuffer))) return FALSE;
  }
  for (dwIndex = 0;  dwIndex < PRF_OUT_LEN;  dwIndex++)
    pbOut[dwIndex] ^= pbBuffer[dwIndex];
  return TRUE;
}

BOOL SpcPBKDF2(BYTE *pbPassword, DWORD cbPassword, BYTE *pbSalt, DWORD cbSalt,
               DWORD dwIterations, BYTE *pbOut, DWORD cbOut) {
  BOOL       bResult = FALSE;
  BYTE       pbFinal[PRF_OUT_LEN];
  DWORD      dwBlock, dwBlockCount, dwLeftOver;
  HCRYPTKEY  hKey;
  HCRYPTPROV hProvider;

  if (cbOut > ((((__int64)1) << 32) - 1) * PRF_OUT_LEN) return FALSE;
  if (!(hProvider = SpcGetExportableContext())) return FALSE;
  if (!(hKey = SpcImportKeyData(hProvider, CALG_RC4, pbPassword, cbPassword))) {
    CryptReleaseContext(hProvider, 0);
    return FALSE;
  }

  dwBlockCount = cbOut / PRF_OUT_LEN;
  dwLeftOver   = cbOut % PRF_OUT_LEN;
  for (dwBlock = 1;  dwBlock <= dwBlockCount;  dwBlock++) {
    if (!PKCS5FinalPRF(hProvider, hKey, pbSalt, cbSalt, dwIterations, dwBlock,
                       pbOut + (dwBlock - 1) * PRF_OUT_LEN)) goto done;
  }
  if (dwLeftOver) {
    SecureZeroMemory(pbFinal, PRF_OUT_LEN);
    if (!PKCS5FinalPRF(hProvider, hKey, pbSalt, cbSalt, dwIterations, dwBlock,
                       pbFinal)) goto done;
    CopyMemory(pbOut + (dwBlock - 1) * PRF_OUT_LEN, pbFinal, dwLeftOver);
  }
  bResult = TRUE;

done:
  CryptDestroyKey(hKey);
  CryptReleaseContext(hProvider, hKey);
  return bResult;
}
