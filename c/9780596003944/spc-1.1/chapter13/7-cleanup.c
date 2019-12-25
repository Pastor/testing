void spc_threadpool_cleanup(spc_threadpool_t *pool) {
  spc_threadpool_task *next;

  SPC_ACQUIRE_MUTEX(threadpool_mutex);
  if (pool->tids) {
    while (pool->tasks) {
      next = pool->tasks->next;
      free(pool->tasks);
      pool->tasks = next;
    }
    free(pool->tids);
    pool->tids = 0;
  }
  pool->destroy = pool->size;
  SPC_BROADCAST_COND(pool->cond);
  SPC_RELEASE_MUTEX(threadpool_mutex);
}
