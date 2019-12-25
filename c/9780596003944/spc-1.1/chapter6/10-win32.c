#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>

void SpcIncrementalHMAC(BYTE *pbKey, DWORD cbKey, ALG_ID Algid) {
  BYTE       out[20];
  DWORD      cbData = sizeof(out), i;
  HCRYPTKEY  hKey;
  HMAC_INFO  HMACInfo;
  HCRYPTHASH hHash;
  HCRYPTPROV hProvider;

  hProvider = SpcGetExportableContext();
  hKey = SpcImportKeyData(hProvider, Algid, pbKey, cbKey);
  CryptCreateHash(hProvider, CALG_HMAC, hKey, 0, &hHash);

  HMACInfo.HashAlgid     = CALG_SHA1;
  HMACInfo.pbInnerString = HMACInfo.pbOuterString = 0;
  HMACInfo.cbInnerString = HMACInfo.cbOuterString = 0;
  CryptSetHashParam(hHash, HP_HMAC_INFO, (BYTE *)&HMACInfo, 0);

  CryptHashData(hHash, (BYTE *)"fred", 4, 0);
  CryptGetHashParam(hHash, HP_HASHVAL, out, &cbData, 0);
  for (i = 0;  i < cbData;  i++) printf("%02x", out[i]);
  printf("\n");

  CryptDestroyHash(hHash);
  CryptDestroyKey(hKey);
  CryptReleaseContext(hProvider, 0);
}
