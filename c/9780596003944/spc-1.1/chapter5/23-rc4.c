void RC4(RC4_CTX *c, size_t n, unsigned char *in, unsigned char *out) {
  unsigned char swap;

  while (n--) {
    c->j += c->sbox[++c->i];
    swap = c->sbox[c->i];
    c->sbox[c->i] = c->sbox[c->j];
    c->sbox[c->j] = swap;
    swap = c->sbox[c->i] + c->sbox[c->j];
    *out++ = *in++ ^ c->sbox[swap];
  }
}
