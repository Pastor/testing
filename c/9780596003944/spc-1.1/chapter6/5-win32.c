#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  BYTE          *pbData;
  DWORD         cbData = sizeof(DWORD), cbHashSize, i;
  HCRYPTHASH    hSHA1;
  HCRYPTPROV    hProvider;
  unsigned char *s1 = "Testing";
  unsigned char *s2 = "...1...2...3...";

  CryptAcquireContext(&hProvider, 0, MS_DEF_PROV, PROV_RSA_FULL, 0);
  CryptCreateHash(hProvider, CALG_SHA1, 0, 0, &hSHA1);
  CryptHashData(hSHA1, s1, strlen(s1), 0);
  CryptHashData(hSHA1, s2, strlen(s2), 0);
  CryptGetHashParam(hSHA1, HP_HASHSIZE, (BYTE *)&cbHashSize, &cbData, 0);
  pbData = (BYTE *)LocalAlloc(LMEM_FIXED, cbHashSize);
  CryptGetHashParam(hSHA1, HP_HASHVAL, pbData, &cbHashSize, 0);
  CryptDestroyHash(hSHA1);
  CryptReleaseContext(hProvider, 0);

  printf("SHA1(\"%s%s\") = ", s1, s2);
  for (i = 0;  i < cbHashSize;  i++) printf("%02x", pbData[i]);
  printf("\n");

  LocalFree(pbData);
  return 0;
}
