#include <stdio.h>
#include <string.h>

char *get_filename(int n, char *buf, int buf_len) {
  int  x;
  char *p;

  buf[0] = 0;
  p = &((char *)&n)[0];
  for (x = 0;  x < 4;  x++, p++) {
    switch (*p) {
      case 1:
        strncat(buf, "swd", buf_len - strlen(buf));
        break;
      case 2:
        strncat( buf, "no", buf_len - strlen(buf));
        break;
      case 3:
        strncat( buf, "/e", buf_len - strlen(buf));
        break;
      case 4:
        strncat( buf, "as", buf_len - strlen(buf));
        break;
      case 5:
        strncat( buf, "us", buf_len - strlen(buf));
        break;
      case 6:
        strncat( buf, "tc/p", buf_len - strlen(buf));
        break;
      case 7:
        strncat( buf, "mp", buf_len - strlen(buf));
        break;
      default:
        strncat( buf, "/", buf_len);
    }
  }
  buf[buf_len] = 0;
  return buf;
}

int main(int argc, char *argv[]) {
  char filename[32];

  /* 0x01040603 is 03 . 06 . 04 . 01 -- note that the number is passed as little
   * endian but read as big endian!
   */
  printf("get_filename() returns \"%s\"\n",
         get_filename(0x01040603, filename, sizeof(filename)));
  return 0;
}
