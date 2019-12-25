static void cleanup_worker(void *arg) {
  spc_threadpool_t *pool = (spc_threadpool_t *)arg;

  if (pool->destroy && !--pool->destroy) {
    SPC_DESTROY_COND(pool->cond);
    free(pool);
  }
  SPC_RELEASE_MUTEX(threadpool_mutex);
}

#ifndef WIN32
static void *worker_thread(void *arg) {
#else
static DWORD WINAPI worker_thread(LPVOID arg) {
#endif
  int                 done = 0;
  spc_threadpool_t    *pool = (spc_threadpool_t *)arg;
  spc_threadpool_task *task;

  while (!done) {
    SPC_ACQUIRE_MUTEX(threadpool_mutex);
    if (!pool->tids || pool->destroy) {
      cleanup_worker(arg);
      return 0;
    }
    SPC_CLEANUP_PUSH(cleanup_worker, arg);
    if (pool->tids) {
      if (!pool->tasks) SPC_WAIT_COND(pool->cond, threadpool_mutex);
      if ((task = pool->tasks) != 0)
        if (!(pool->tasks = task->next)) pool->tail = 0;
    } else done = 1;
    SPC_CLEANUP_POP(1);

    if (!done && task) {
      task->fnptr(task->arg);
      free(task);
    }
  }
  return 0;
}
