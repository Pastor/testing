#include <stdio.h>

/* warning: replace "crc32_stored" with the real checksum! */
asm(".long 0xCEFAEDFE \n"   /* look for 0xFEEDFACE markers */
    "crc32_stored:    \n"
    ".long 0xFFFFFFFF \n"   /* change this in the binary! */
    ".long 0xCEFAEDFE \n"   /* end marker */
);

CRC_START_BLOCK(test)
int test_routine(int a) {
  while (a < 12) a = (a - (a * 3)) + 1;
  return a;
}
CRC_END_BLOCK( test )

int main(int argc, char *argv[]) {
  unsigned long crc;

  crc = crc32_calc(CRC_BLOCK_ADDR(test), CRC_BLOCK_LEN(test));
#ifdef TEST_BUILD
  /* This printf() displays the CRC value that needs to be stored in the program.
   * The printf() must be removed, and the program recompiled before distribution.
   */
  printf("CRC is %08X\n", crc);
#else
  if (crc != crc32_stored) {
    printf("CRC32 %#08X does not match %#08X\n", crc, crc32_stored);
    return 1;
  }
  printf("CRC32 %#08X is OK\n", crc);
#endif
  return 0;
}
