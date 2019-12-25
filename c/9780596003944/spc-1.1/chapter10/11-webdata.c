static BYTE *RetrieveWebData(LPSTR lpszURL, DWORD *lpdwDataLength) {
  DWORD     dwContentLength, dwFlags, dwNumberOfBytesRead,
            dwNumberOfBytesToRead;
  LPVOID    lpBuffer, lpFullBuffer, lpNewBuffer;
  HINTERNET hRequest, hSession;

  hSession = InternetOpen(TEXT("Secure Programming Cookbook Recipe 10.11"),
                          INTERNET_OPEN_TYPE_PROXY, 0, 0, 0);
  if (!hSession) return 0;

  dwFlags = INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
            INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS | INTERNET_FLAG_NO_COOKIES |
            INTERNET_FLAG_NO_UI | INTERNET_FLAG_PASSIVE;
  hRequest = InternetOpenUrl(hSession, lpszURL, 0, 0, dwFlags, 0);
  if (!hRequest) {
    InternetCloseHandle(hSession);
    return 0;
  }

  dwContentLength = 0;
  dwNumberOfBytesToRead = 1024;
  lpFullBuffer = lpBuffer = LocalAlloc(LMEM_FIXED, dwNumberOfBytesToRead);
  while (InternetReadFile(hRequest, lpBuffer, dwNumberOfBytesToRead,
                          &dwNumberOfBytesRead)) {
    dwContentLength = dwContentLength + dwNumberOfBytesRead;
    if (dwNumberOfBytesRead != dwNumberOfBytesToRead) break;
    if (!(lpNewBuffer = LocalReAlloc(lpFullBuffer, dwContentLength +
                                     dwNumberOfBytesToRead, 0))) {
      LocalFree(lpFullBuffer);
      InternetCloseHandle(hRequest);
      InternetCloseHandle(hSession);
      return 0;
    }
    lpFullBuffer = lpNewBuffer;
    lpBuffer = (LPVOID)((LPBYTE)lpFullBuffer + dwContentLength);
  }

  if ((lpNewBuffer = LocalReAlloc(lpFullBuffer, dwContentLength, 0)) != 0)
    lpFullBuffer = lpNewBuffer;
  InternetCloseHandle(hRequest);
  InternetCloseHandle(hSession);
  *lpdwDataLength = dwContentLength;
  return (BYTE *)lpFullBuffer;
}
