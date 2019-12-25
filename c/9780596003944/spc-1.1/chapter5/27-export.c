#include <windows.h>
#include <wincrypt.h>

BYTE *SpcExportKeyData(HCRYPTPROV hProvider, HCRYPTKEY hKey, DWORD *cbData) {
  BOOL      bResult = FALSE;
  BYTE      *pbData = 0, *pbKeyData;
  HCRYPTKEY hExpKey = 0;

  if (!CryptGetUserKey(hProvider, AT_KEYEXCHANGE, &hExpKey)) {
    if (GetLastError() != NTE_NO_KEY) goto done;
    if (!CryptGenKey(hProvider, AT_KEYEXCHANGE, (1024 << 16), &hExpKey))
      goto done;
  }

  if (!CryptExportKey(hKey, hExpKey, SIMPLEBLOB, 0, 0, cbData)) goto done;
  if (!(pbData = (BYTE *)LocalAlloc(LMEM_FIXED, *cbData))) goto done;
  if (!CryptExportKey(hKey, hExpKey, SIMPLEBLOB, 0, pbData, cbData))
    goto done;

  pbKeyData = pbData + sizeof(BLOBHEADER) + sizeof(ALG_ID);
  (*cbData) -= (sizeof(BLOBHEADER) + sizeof(ALG_ID));
  bResult = CryptDecrypt(hExpKey, 0, TRUE, 0, pbKeyData, cbData);

done:
  if (hExpKey) CryptDestroyKey(hExpKey);
  if (!bResult && pbData) LocalFree(pbData);
  else if (pbData) MoveMemory(pbData, pbKeyData, *cbData);
  return (bResult ? (BYTE *)LocalReAlloc(pbData, *cbData, 0) : 0);
}
