#include <stddef.h>

#define INTERLEAVE_SIZE 512

unsigned char *spc_double_mac(unsigned char *text, size_t len,
                              unsigned char key[16]) {
  SPC_OMAC_CTX  ctx1, ctx2;
  unsigned char *out = (unsigned char *)malloc(16);
  unsigned char tmp[16];

  if (!out) abort(); /* Consider throwing an exception instead. */
  spc_omac1_init(&ctx1, key, 16);
  spc_omac1_init(&ctx2, key, 16);
  while (len > 2 * INTERLEAVE_SIZE) {
    spc_omac_update(&ctx1, text, INTERLEAVE_SIZE);
    spc_omac_update(&ctx2, text + INTERLEAVE_SIZE, INTERLEAVE_SIZE);
    text += 2 * INTERLEAVE_SIZE;
    len  -= 2 * INTERLEAVE_SIZE;
  }
  if (len > INTERLEAVE_SIZE) {
    spc_omac_update(&ctx1, text, INTERLEAVE_SIZE);
    spc_omac_update(&ctx2, text + INTERLEAVE_SIZE, len - INTERLEAVE_SIZE);
  } else spc_omac_update(&ctx1, text, len);
  spc_omac_final(&ctx1, tmp);
  spc_omac_update(&ctx2, tmp, sizeof(tmp));
  spc_omac_final(&ctx2, out);
  return out;
}
