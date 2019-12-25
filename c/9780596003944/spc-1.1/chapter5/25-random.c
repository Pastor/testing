HCRYPTKEY SpcGetRandomKey(HCRYPTPROV hProvider, ALG_ID Algid, DWORD dwSize) {
  DWORD     dwFlags;
  HCRYPTKEY hKey;

  dwFlags = ((dwSize << 16) & 0xFFFF0000) | CRYPT_EXPORTABLE;
  if (!CryptGenKey(hProvider, Algid, dwFlags, &hKey)) return 0;
  return hKey;
}
