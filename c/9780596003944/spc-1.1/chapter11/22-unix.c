#include <pthread.h>

static void *thread_stub(void *arg) {
  return 0;
}

void spc_time_threads(unsigned int numiters) {
  pthread_t tid;

  while (numiters--)
    if (!pthread_create(&tid, 0, thread_stub, 0))
      pthread_join(tid, 0);
}
