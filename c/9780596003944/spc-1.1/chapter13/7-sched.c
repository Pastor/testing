#include <stdlib.h>
#ifndef WIN32
#include <pthread.h>
#else
#include <windows.h>
#endif

typedef void (*spc_threadpool_fnptr)(void *);

typedef struct _spc_threadpool_task {
  spc_threadpool_fnptr        fnptr;
  void                        *arg;
  struct _spc_threadpool_task *next;
} spc_threadpool_task;

typedef struct {
  int                 size;
  int                 destroy;
#ifndef WIN32
  pthread_t           *tids;
  pthread_cond_t      cond;
#else
  HANDLE              *tids;
  HANDLE              cond;
#endif
  spc_threadpool_task *tasks;
  spc_threadpool_task *tail;
} spc_threadpool_t;

#ifndef WIN32
#define SPC_ACQUIRE_MUTEX(mtx)       pthread_mutex_lock(&(mtx))
#define SPC_RELEASE_MUTEX(mtx)       pthread_mutex_unlock(&(mtx))
#define SPC_CREATE_COND(cond)        pthread_cond_init(&(cond), 0)
#define SPC_DESTROY_COND(cond)       pthread_cond_destroy(&(cond))
#define SPC_SIGNAL_COND(cond)        pthread_cond_signal(&(cond))
#define SPC_BROADCAST_COND(cond)     pthread_cond_broadcast(&(cond))
#define SPC_WAIT_COND(cond, mtx)     pthread_cond_wait(&(cond), &(mtx))
#define SPC_CLEANUP_PUSH(func, arg)  pthread_cleanup_push(func, arg)
#define SPC_CLEANUP_POP(exec)        pthread_cleanup_pop(exec)
#define SPC_CREATE_THREAD(t, f, arg) (!pthread_create(&(t), 0, (f), (arg)))

static pthread_mutex_t threadpool_mutex = PTHREAD_MUTEX_INITIALIZER;
#else
#define SPC_ACQUIRE_MUTEX(mtx)       WaitForSingleObjectEx((mtx), INFINITE, FALSE)
#define SPC_RELEASE_MUTEX(mtx)       ReleaseMutex((mtx))
#define SPC_CREATE_COND(cond)        (cond) = CreateEvent(0, TRUE, FALSE, 0)
#define SPC_DESTROY_COND(cond)       CloseHandle((cond))
#define SPC_SIGNAL_COND(cond)        SetEvent((cond))
#define SPC_BROADCAST_COND(cond)     PulseEvent((cond))
#define SPC_WAIT_COND(cond, mtx)     spc_win32_wait_cond((cond), (mtx))
#define SPC_CLEANUP_PUSH(func, arg)  { void (*__spc_func)(void *) = (func); \
                                     void *__spc_arg = (arg)
#define SPC_CLEANUP_POP(exec)        if ((exec)) __spc_func(__spc_arg); } \
                                     do {  } while (0)
#define SPC_CREATE_THREAD(t, f, arg) ((t) = CreateThread(0, 0, (f), (arg), 0, 0))

static HANDLE threadpool_mutex = 0;
#endif

#ifdef WIN32
static void spc_win32_wait_cond(HANDLE cond, HANDLE mutex) {
  HANDLE handles[2];

  handles[0] = cond;
  handles[1] = mutex;
  ResetEvent(cond);
  ReleaseMutex(mutex);
  WaitForMultipleObjectsEx(2, handles, TRUE, INFINITE, FALSE);
}
#endif

int spc_threadpool_schedule(spc_threadpool_t *pool, spc_threadpool_fnptr fnptr,
                            void *arg) {
  spc_threadpool_task *task;

  SPC_ACQUIRE_MUTEX(threadpool_mutex);
  if (!pool->tids) {
    SPC_RELEASE_MUTEX(threadpool_mutex);
    return 0;
  }
  if (!(task = (spc_threadpool_task *)malloc(sizeof(spc_threadpool_task)))) {
    SPC_RELEASE_MUTEX(threadpool_mutex);
    return 0;
  }
  task->fnptr = fnptr;
  task->arg   = arg;
  task->next  = 0;
  if (pool->tail) pool->tail->next = task;
  else pool->tasks = task;
  pool->tail = task;
  SPC_SIGNAL_COND(pool->cond);
  SPC_RELEASE_MUTEX(threadpool_mutex);
  return 1;
}
