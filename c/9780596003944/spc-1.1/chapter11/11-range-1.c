#include <limits.h>
#include <stdlib.h>

int spc_rand_range(int min, int max) {
  unsigned int rado;
  int          range = max - min + 1;

  if (max < min) abort(); /* Do your own error handling if appropriate.*/
  do {
    rado = spc_rand_uint();
  } while (rado > UINT_MAX - (UINT_MAX % range));
  return min + (rado % range);
}
