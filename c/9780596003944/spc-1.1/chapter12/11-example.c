#include <stdio.h>

int main(int argc, char *argv[]) {
  char str[] = {
    A('/'), A('e'), A('t'), A('c'), A('/'),
    A('p'), A('a'), A('s'), A('s'), A('w'), A('d'), 0
  };

  UNHIDE_STR(str);
  printf("%s\n", str);
  HIDE_STR(str);

  return 0;
}
