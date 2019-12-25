#include <errno.h>
#include <sys/types.h>
#ifndef WIN32
#include <sys/socket.h>
#include <pthread.h>
#else
#include <windows.h>
#include <winsock.h>
#endif

#ifndef WIN32
#define SPC_ACQUIRE_MUTEX(mtx)      pthread_mutex_lock(&(mtx))
#define SPC_RELEASE_MUTEX(mtx)      pthread_mutex_unlock(&(mtx))
#define SPC_CREATE_COND(cond)       (!pthread_cond_init(&(cond), 0))
#define SPC_DESTROY_COND(cond)      pthread_cond_destroy(&(cond))
#define SPC_SIGNAL_COND(cond)       pthread_cond_signal(&(cond))
#define SPC_BROADCAST_COND(cond)    pthread_cond_broadcast(&(cond))
#define SPC_WAIT_COND(cond, mtx)    pthread_cond_wait(&(cond), &(mtx))
#define SPC_CLEANUP_PUSH(func, arg) pthread_cleanup_push(func, arg)
#define SPC_CLEANUP_POP(exec)       pthread_cleanup_pop(exec)
#define closesocket(sock)           close((sock))
#define SOCKET_ERROR                -1
#else
#define SPC_ACQUIRE_MUTEX(mtx)      WaitForSingleObjectEx((mtx), INFINITE, FALSE)
#define SPC_RELEASE_MUTEX(mtx)      ReleaseMutex((mtx))
#define SPC_CREATE_COND(cond)       ((cond) = CreateEvent(0, TRUE, FALSE, 0))
#define SPC_DESTROY_COND(cond)      CloseHandle((cond))
#define SPC_SIGNAL_COND(cond)       SetEvent((cond))
#define SPC_BROADCAST_COND(cond)    PulseEvent((cond))
#define SPC_WAIT_COND(cond, mtx)    spc_win32_wait_cond((cond), (mtx))
#define SPC_CLEANUP_PUSH(func, arg) { void (*__spc_func)(void *) = func; \
                                    void *__spc_arg = arg;
#define SPC_CLEANUP_POP(exec)       if ((exec)) __spc_func(__spc_arg); } \
                                    do {  } while (0)
#endif

static int              socketpool_used  = 0;
static int              socketpool_limit = 0;

#ifndef WIN32
static pthread_cond_t   socketpool_cond  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t  socketpool_mutex = PTHREAD_MUTEX_INITIALIZER;
#else
static HANDLE           socketpool_cond, socketpool_mutex;
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

int spc_socketpool_init(void) {
#ifdef WIN32
  if (!SPC_CREATE_COND(socketpool_cond)) return 0;
  if (!(socketpool_mutex = CreateMutex(0, FALSE, 0))) {
    CloseHandle(socketpool_cond);
    return 0;
  }
#endif
  return 1;
}

int spc_socketpool_setlimit(int limit) {
  SPC_ACQUIRE_MUTEX(socketpool_mutex);
  if (socketpool_limit > 0 && socketpool_used >= socketpool_limit) {
    if (limit <= 0 || limit > socketpool_limit)
      SPC_BROADCAST_COND(socketpool_cond);
  }
  socketpool_limit = limit;
  SPC_RELEASE_MUTEX(socketpool_mutex);
  return 1;
}
