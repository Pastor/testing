#include <stdio.h>
#include <string.h>

#define BYTES_PER_GROUP 4
#define GROUPS_PER_LINE 4

/* Don't change these */
#define BYTES_PER_LINE (BYTES_PER_GROUP * GROUPS_PER_LINE)

void spc_print_hex(char *prefix, unsigned char *str, int len) {
  unsigned long i, j, preflen = 0;

  if (prefix) {
    printf("%s", prefix);
    preflen = strlen(prefix);
  }

  for (i = 0;  i < len;  i++) {
    printf("%02X ", str[i]);
    if (((i % BYTES_PER_LINE) == (BYTES_PER_LINE - 1)) && ((i + 1) != len)) {
      putchar('\n');
      for (j = 0;  j < preflen;  j++) putchar(' ');
    }
    else if ((i % BYTES_PER_GROUP) == (BYTES_PER_GROUP - 1)) putchar(' ');
  }
  putchar('\n');
}
