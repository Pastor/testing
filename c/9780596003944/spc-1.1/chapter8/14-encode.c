char *spc_cookie_encode(char *cookie, unsigned char *nonce) {
  size_t        cookielen;
  unsigned char *out;

  cookielen = strlen(cookie);
  if (!(out = (unsigned char *)malloc(cookielen + 16))) return 0;
  if (!nonce) nonce = spc_cookie_nonce;

  cwc_encrypt_message(&spc_cookie_cwc, 0, 0, cookie, cookielen, nonce, out);
  cookie = spc_base64_encode(out, cookielen + 16, 0);

  free(out);
  return cookie;
}
