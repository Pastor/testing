#include <stdlib.h>
#include <errno.h>
#include <cwc.h>

#define SPC_CLIENT_DISTINGUISHER 0x80
#define SPC_SERVER_DISTINGUISHER 0x00
#define SPC_SERVER_LACKS_NONCE   0xff

#define SPC_IV_IX   1
#define SPC_CTR_IX  6
#define SPC_IV_LEN  5
#define SPC_CTR_LEN 5

#define SPC_CWC_NONCE_LEN (SPC_IV_LEN + SPC_CTR_LEN + 1)

typedef struct {
  cwc_t         cwc;
  unsigned char nonce[SPC_CWC_NONCE_LEN];
  int           fd;
} spc_ssock_t;
