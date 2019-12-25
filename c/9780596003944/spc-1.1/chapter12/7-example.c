#include <stdlib.h>

char g1, g2; /* store half of the integer here */

void init_rand(char a, char b) {
  srand(REBUILD_VAR(a, g1, b, g2));
}

int main(int argc, char *argv[]) {
  int  seed = 0x81206583;
  char a, b;

  SPLIT_VAR(seed, a, g1, b, g2);
  init_rand(a, b);

  return 0;
}
