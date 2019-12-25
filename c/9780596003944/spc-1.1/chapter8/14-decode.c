char *spc_cookie_decode(char *data, unsigned char *nonce) {
  int           error;
  char          *out;
  size_t        cookielen;
  unsigned char *cookie;

  if (!(cookie = spc_base64_decode(data, &cookielen, 1, &error))) return 0;
  if (!(out = (char *)malloc(cookielen - 16 + 1))) {
    free(cookie);
    return 0;
  }
  if (!nonce) nonce = spc_cookie_nonce;

  error = !cwc_decrypt_message(&spc_cookie_cwc, 0, 0, cookie, cookielen,
                               nonce, out);
  free(cookie);
  if (error) {
    free(out);
    return 0;
  }

  out[cookielen - 16] = 0;
  return out;
}
