#include <stdbool.h>
#include <stdlib.h>

#if defined(_MSC_VER)
#include <Windows.h>
//https://docs.microsoft.com/ru-ru/windows/desktop/Sync/using-condition-variables

struct Event {
    CRITICAL_SECTION cs;
    CONDITION_VARIABLE cv;
};

#else
#include <unistd.h>
#include <time.h>
#include <pthread.h>

struct Event {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
};

#endif

void
u_delay(long milliseconds) {

#if defined(_MSC_VER)
    Sleep(milliseconds);
#else
    usleep((useconds_t)(milliseconds * 1000));
#endif
}

void
signal_init(volatile void **busy) {
    struct Event *event = calloc(sizeof(struct Event), 1);
#if defined(_MSC_VER)
    InitializeConditionVariable(&event->cv);
    InitializeCriticalSection(&event->cs);
#else
    pthread_cond_init(&event->cond, 0);
    pthread_mutex_init(&event->mutex, 0);
#endif
    (*busy) = event;
}

void
signal_destroy(volatile void **busy) {
    if (busy != NULL && (*busy) != NULL) {
#if defined(_MSC_VER)

#else
        struct Event *event = (struct Event *) (*busy);
        pthread_cond_destroy(&event->cond);
        pthread_mutex_destroy(&event->mutex);
#endif
        (*busy) = NULL;
    }
}

bool
signal_try_lock(volatile void *busy) {
    if (busy != NULL) {
        struct Event *event = (struct Event *) busy;
#if defined(_MSC_VER)
        return TryEnterCriticalSection(&event->cs) != 0;
#else
        if (pthread_mutex_trylock(&event->mutex) == 0)
            return true;
#endif
    }
    return false;
}

void
signal_do(volatile void *busy) {
    if (busy != NULL) {
        struct Event *event = (struct Event *) busy;
#if defined(_MSC_VER)
        WakeConditionVariable(&event->cv);
        LeaveCriticalSection(&event->cs);
#else
        pthread_cond_signal(&event->cond);
        pthread_mutex_unlock(&event->mutex);
#endif
    }
}

void
signal_unlock(volatile void *busy) {
    if (busy != NULL) {
        struct Event *event = (struct Event *) busy;
#if defined(_MSC_VER)
        LeaveCriticalSection(&event->cs);
#else
        pthread_mutex_unlock(&event->mutex);
#endif
    }
}

bool
signal_wait(volatile void *busy, int wait_milliseconds) {
    if (busy != NULL) {
        struct Event *event = (struct Event *) busy;
#if defined(_MSC_VER)
        EnterCriticalSection(&event->cs);
        return SleepConditionVariableCS(&event->cv, &event->cs, INFINITE) != 0;
#else
        if (pthread_mutex_lock(&event->mutex) == 0) {
            return pthread_cond_wait(&event->cond, &event->mutex) == 0;
        }
#endif
    }
    return false;
}

void
busy_init(volatile void **busy) {
#if defined(_MSC_VER)
    (*busy) = malloc(sizeof(LONG));
#else
    (*busy) = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(((pthread_mutex_t *) (*busy)), NULL);
    pthread_mutex_unlock(((pthread_mutex_t *) (*busy)));
#endif
}

void
busy_destroy(volatile void **busy) {
#if defined(_MSC_VER)
    if ((*busy) != NULL) {
        free((void *) (*busy));
        (*busy) = NULL;
    }
#else
    if ((*busy) != NULL) {
        pthread_mutex_destroy(((pthread_mutex_t *) (*busy)));
        free((void *) (*busy));
        (*busy) = NULL;
    }
#endif
}

bool
busy_try_lock(volatile void *busy) {
    if (busy == 0)
        return false;
#if defined(_MSC_VER)
    if (InterlockedCompareExchange((LONG *) busy, 1, 0))
        return true;
#else
    if (pthread_mutex_trylock(((pthread_mutex_t *) busy)) == 0)
        return true;
#endif
    return false;
}

void
busy_unlock(volatile void *busy) {
#if defined(_MSC_VER)
    InterlockedExchange((LONG *) busy, 0);
#else
    pthread_mutex_unlock(((pthread_mutex_t *) busy));
#endif
}

