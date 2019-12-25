spc_threadpool_t *spc_threadpool_init(int pool_size) {
  int              i;
  spc_threadpool_t *pool;

#ifdef WIN32
  if (!threadpool_mutex) threadpool_mutex = CreateMutex(NULL, FALSE, 0);
#endif

  if (!(pool = (spc_threadpool_t *)malloc(sizeof(spc_threadpool_t))))
    return 0;
#ifndef WIN32
  pool->tids = (pthread_t *)malloc(sizeof(pthread_t) * pool_size);
#else
  pool->tids = (HANDLE *)malloc(sizeof(HANDLE) * pool_size);
#endif
  if (!pool->tids) {
    free(pool);
    return 0;
  }
  SPC_CREATE_COND(pool->cond);

  pool->size    = pool_size;
  pool->destroy = 0;
  pool->tasks   = 0;
  pool->tail    = 0;

  SPC_ACQUIRE_MUTEX(threadpool_mutex);
  for (i = 0;  i < pool->size;  i++) {
    if (!SPC_CREATE_THREAD(pool->tids[i], worker_thread, pool)) {
      pool->destroy = i;
      free(pool->tids);
      pool->tids = 0;
      SPC_RELEASE_MUTEX(threadpool_mutex);
      return 0;
    }
  }
  SPC_RELEASE_MUTEX(threadpool_mutex);
  return pool;
}
