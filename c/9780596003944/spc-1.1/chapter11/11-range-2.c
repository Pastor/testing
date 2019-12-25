#include <stdlib.h>

int spc_rand_range(int min, int max) {
  if (max < min) abort();
  return min + (spc_rand_uint() % (max - min + 1));
}
