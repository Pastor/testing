#include <stdio.h>

void my_func(void) {
  printf("my_func() called!\n");
}

SET_FN_PTR(my_func, 0x01301100); /* 0x01301100 is some arbitrary value */

int main(int argc, char *argv[]) {
  void (*ptr)(void);

  ptr = GET_FN_PTR(my_func);     /* get the real address of the function */
  (*ptr)();                      /* make the function call */

  return 0;
}
