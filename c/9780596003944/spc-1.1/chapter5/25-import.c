HCRYPTKEY SpcImportKey(HCRYPTPROV hProvider, BYTE *pbData, DWORD dwDataLen,
                       HCRYPTKEY hPublicKey) {
  HCRYPTKEY  hKey;

  if (!CryptImportKey(hProvider, pbData, dwDataLen, hPublicKey, CRYPT_EXPORTABLE,
                      &hKey)) return 0;
  return hKey;
}
