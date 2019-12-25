#include <stdio.h>

void my_func(void) {
  int x;

  SPC_DEFINE_DBG_SYM(myfunc_nodebug);
  for (x = 0;  x < 10;  x++) printf("X!\n");
}

SPC_USE_DBG_SYM(myfunc_nodebug);

int main(int argc, char *argv[]) {
  if (spc_check_int3(myfunc_nodebug)) printf("being debugged: int3!\n");
  return(0);
}
