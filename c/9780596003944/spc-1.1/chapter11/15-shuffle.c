#include <stdlib.h>

void spc_shuffle(int *items, size_t numitems) {
  int    tmp;
  size_t swapwith;

  while (--numitems) {
    /* Remember, it must be possible for a value to swap with itself */
    swapwith = spc_rand_range(0, numitems);
    tmp = items[swapwith];
    items[swapwith] = items[numitems];
    items[numitems] = tmp;
  }
}
