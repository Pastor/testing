#include <sys/types.h>  /* for netinet/in.h on some platforms */
#include <netinet/in.h> /* for arpa/inet.h on some platforms */
#include <arpa/inet.h>  /* for htonl() */
#include <openssl/evp.h>
#include <openssl/hmac.h>

#define HMAC_OUT_LEN  20 /* SHA1 specific */

void spc_make_derived_key(unsigned char *base, size_t bl, unsigned char *dist,
                          size_t dl, unsigned char *out, size_t ol) {
  HMAC_CTX       c;
  unsigned  long ctr = 0, nbo_ctr;
  size_t         tl, i;
  unsigned char  last[HMAC_OUT_LEN];

  while (ol >= HMAC_OUT_LEN) {
    HMAC_Init(&c,  base, bl, EVP_sha1());
    HMAC_Update(&c, dist, dl);
    nbo_ctr = htonl(ctr++);
    HMAC_Update(&c, (unsigned char *)&nbo_ctr, sizeof(nbo_ctr));
    HMAC_Final(&c,  out, &tl);
    out += HMAC_OUT_LEN;
    ol  -= HMAC_OUT_LEN;
  }
  if (!ol) return;
  HMAC_Init(&c, base, bl, EVP_sha1());
  HMAC_Update(&c, dist, dl);
  nbo_ctr = htonl(ctr);
  HMAC_Update(&c, (unsigned char *)&nbo_ctr, sizeof(nbo_ctr));
  HMAC_Final(&c, last, &tl);
  for (i = 0;  i < ol;  i++)
    out[i] = last[i];
}
