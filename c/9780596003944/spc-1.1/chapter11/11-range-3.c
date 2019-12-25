#include <limits.h>

int spc_rand_range(int min, int max) {
  if (max < min) abort();
  return min + (int)((double)spc_rand_uint() *
                     (max - min + 1) / (double)UINT_MAX) % (max - min);
}
