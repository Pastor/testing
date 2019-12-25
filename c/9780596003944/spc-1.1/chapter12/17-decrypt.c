#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#define SPC_SMC_START_BLOCK(label)  void label(void) {  }
#define SPC_SMC_END_BLOCK(label)    void _##label(void) {  }
#define SPC_SMC_BLOCK_LEN(label)    (int)_##label - (int)label
#define SPC_SMC_BLOCK_ADDR(label)   (unsigned char *)label
#define SPC_SMC_START_KEY(label)    void key_##label(void) { }
#define SPC_SMC_END_KEY(label)      void _key_##label(void) { }
#define SPC_SMC_KEY_LEN(label)      (int)_key_##label - (int)key_##label
#define SPC_SMC_KEY_ADDR(label)     (unsigned char *)key_##label
#define SPC_SMC_OFFSET(label)       (long)label - (long)_start

extern void _start(void);

/* returns number of bytes encoded */
int spc_smc_decrypt(unsigned char *buf, int buf_len, unsigned char *key, int key_len) {
  RC4_CTX ctx;

  RC4_set_key(&ctx, key_len, key);

  /* NOTE: most code segments have read-only permissions, and so must be modified
   * to allow writing to the buffer
   */
  if (mprotect(buf, buf_len, PROT_WRITE | PROT_READ | PROT_EXEC)) {
    fprintf(stderr, "mprotect: %s\n", strerror(errno));
    return(0);
  }

  /* decrypt the buffer */
  RC4(&ctx, buf_len, buf, buf);

  /* restore the original memory permissions */
  mprotect(buf, buf_len, PROT_READ | PROT_EXEC);

  return(buf_len);
}
