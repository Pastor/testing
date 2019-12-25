#include <stdlib.h>
#include <string.h>

/* last_nonce must be a pointer to a NULL on first invocation. */
int spc_omac1_validate(unsigned char *ct, size_t ctlen,
                       unsigned char sent_nonce[16],
                       unsigned char *sent_tag, unsigned char *k,
                       unsigned char **last_nonce) {
  int           i;
  SPC_OMAC_CTX  c;
  unsigned char calc_tag[16]; /* Maximum tag size for OMAC. */

  spc_omac1_init(&c, k, 16);
  if (*last_nonce) {
    for (i = 0;  i < 16;  i++)
      if (sent_nonce[i] > (*last_nonce)[i]) goto nonce_okay;
    return 0; /* Nonce is equal to or less than last nonce. */
  }
nonce_okay:
  spc_omac_update(&c, sent_nonce, 16);
  spc_omac_update(&c, ct, ctlen);
  spc_omac_final(&c, calc_tag);
  for (i = 0;  i < 16;  i++)
    if (calc_tag[i] != sent_tag[i]) return 0;
  if (sent_nonce) {
    if (!*last_nonce) *last_nonce = (unsigned char *)malloc(16);
    if (!*last_nonce) abort();  /* Consider an exception instead. */
    memcpy(*last_nonce, sent_nonce, 16);
  }
  return 1;
}
