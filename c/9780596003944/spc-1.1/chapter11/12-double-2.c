#include <stdlib.h>

double spc_rand_real_range(double min, double max) {
  if (max < min) abort();
  return spc_rand_real() * (max - min) + min;
}
