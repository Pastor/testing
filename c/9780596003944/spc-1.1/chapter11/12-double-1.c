#include <limits.h>

double spc_rand_real(void) {
  return ((double)spc_rand_uint()) / (double)UINT_MAX;
}
