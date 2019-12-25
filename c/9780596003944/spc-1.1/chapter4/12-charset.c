typedef struct {
  unsigned char *cset;
  int           csetlen;
  unsigned char reverse[256];
  unsigned char maxvalid;
} ENCMAP;

#define decrypt_within_charset encrypt_within_charset

void setup_charset_map(ENCMAP *s, unsigned char *charset, int csetlen) {
  int i;

  s->cset    = charset;
  s->csetlen = csetlen;

  for (i = 0;  i < 256;  i++) s->reverse[i] = -1;
  for (i = 0;  i < csetlen;  i++) s->reverse[charset[i]] = i;
  s->maxvalid = 255 - (256 % csetlen);
}

void encrypt_within_charset(ENCMAP *s, unsigned char *in, long inlen,
                            unsigned char *out, unsigned char (*keystream_byte)()) {
  long          i;
  unsigned char c;

  for (i = 0;  i < inlen;  i++) {
    do {
      c = (*keystream_byte)();
    } while(c > s->maxvalid);
    *out++ = s->cset[(s->reverse[*in++] + c) % s->csetlen];
  }
}
