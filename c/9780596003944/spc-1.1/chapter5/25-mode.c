BOOL SpcSetKeyMode(HCRYPTKEY hKey, DWORD dwMode) {
  return CryptSetKeyParam(hKey, KP_MODE, (BYTE *)&dwMode, 0);
}

#define SpcSetMode_CBC(hKey) SpcSetKeyMode((hKey), CRYPT_MODE_CBC)
#define SpcSetMode_CFB(hKey) SpcSetKeyMode((hKey), CRYPT_MODE_CFB)
#define SpcSetMode_ECB(hKey) SpcSetKeyMode((hKey), CRYPT_MODE_ECB)
#define SpcSetMode_OFB(hKey) SpcSetKeyMode((hKey), CRYPT_MODE_OFB)
