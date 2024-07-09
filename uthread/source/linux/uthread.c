#include "include/uthread.h"

#include <pthread.h>
#include <stdint.h>
#include <time.h>

#define RET_SUCCESS (0)
#define RET_FAILURE (1)

typedef void* (*start_routine)(void*);

typedef struct {
  pthread_t       handle;
  pthread_t       id;
  start_routine   func;
  pthread_attr_t* attr;
  void*           arg;
} thread_t;

int32_t uthread_create(void** pphandle, void* pattr, void* pfunc, void* parg) {
  if (NULL == pfunc) {
    LOGE("Error: thread function is not specified!");
    return UTHREAD_FAILURE;
  }
  thread_t* phandle = (thread_t*)malloc(sizeof(thread_t));
  if (NULL == phandle) {
    LOGE("Error: failed to allocate memory!");
    return UTHREAD_FAILURE;
  }

  if (pattr) {
    phandle->attr = (pthread_attr_t*)pattr;
  } else {
    phandle->attr = NULL;
  }

  phandle->func = (start_routine)pfunc;
  phandle->arg  = parg;

  int ret       = pthread_create(&phandle->handle, phandle->attr, phandle->func,
                                 phandle->arg);
  if (RET_SUCCESS == ret) {
    *pphandle   = (void*)phandle;
    phandle->id = phandle->handle;
  } else {
    LOGE("Error: failed to create a thread!");
    free(phandle);
    phandle = NULL;
    return UTHREAD_FAILURE;
  }

  LOGI("The thread with ID=0x%lx is created", ((thread_t*)(*pphandle))->handle);

  return UTHREAD_SUCCESS;
}

int32_t uthread_join(void* phandle) {
  if (NULL == phandle) {
    LOGE(
        "Error: thread handle is null, please create a thread properly first!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_join(((thread_t*)phandle)->handle, NULL);

  if (RET_SUCCESS != ret) {
    LOGE("Error: failed to join the thread!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_close(void* phandle) {
  if (NULL == phandle) {
    LOGE(
        "Error: thread handle is null, please create a thread properly first!");
    return UTHREAD_FAILURE;
  }

  // store the handle for the purpose of invoking exiting function
  pthread_t handle = ((thread_t*)phandle)->handle;
  LOGI("The thread with ID=0x%lx is about to exit", ((thread_t*)phandle)->id);

  // dellocate memory
  free(phandle);
  phandle = NULL;

  // invoke exiting function of the thread
  pthread_exit(&handle);

  return UTHREAD_SUCCESS;
}

int32_t uthread_id_get(void* phandle, uint64_t* pthread_id) {
  if (NULL == phandle) {
    LOGE(
        "Error: thread handle is null, please create a thread properly first!");
    return UTHREAD_FAILURE;
  }
  if (NULL == pthread_id) {
    LOGE("Error: please pass a valid thread id pointer!");
    return UTHREAD_FAILURE;
  }

  *pthread_id = (uint64_t)((thread_t*)phandle)->handle;

  return UTHREAD_SUCCESS;
}

int32_t uthread_sleep(uint64_t microseconds) {
  struct timespec ts;
  ts.tv_sec  = microseconds / 1000;
  ts.tv_nsec = microseconds % 1000 * 1000000;

  int ret    = nanosleep(&ts, NULL);

  if (ret != 0) {
    LOGE("Error: Failed to sleep for %lu nanoseconds!", microseconds);
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_init(void** pplock) {
  pthread_mutex_t* plock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
  if (NULL == plock) {
    LOGE("Error: Failed to allocate memory for mutex!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_mutex_init(plock, NULL);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to initialize mutex!");
    free(plock);
    plock = NULL;
    return UTHREAD_FAILURE;
  }

  *pplock = (void*)plock;
  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_deinit(void* plock) {
  if (NULL == plock) {
    LOGE("Error: Mutex handle is null!");
    return UTHREAD_FAILURE;
  }

  pthread_mutex_t* pmutex = (pthread_mutex_t*)plock;

  int ret                 = pthread_mutex_destroy(pmutex);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to destroy mutex!");
    return UTHREAD_FAILURE;
  }

  free(pmutex);
  pmutex = NULL;
  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_lock(void* plock) {
  if (NULL == plock) {
    LOGE("Error: Mutex handle is null!");
    return UTHREAD_FAILURE;
  }

  pthread_mutex_t* pmutex = (pthread_mutex_t*)plock;

  int ret                 = pthread_mutex_lock(pmutex);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to lock mutex!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_unlock(void* plock) {
  if (NULL == plock) {
    LOGE("Error: Mutex handle is null!");
    return UTHREAD_FAILURE;
  }

  pthread_mutex_t* pmutex = (pthread_mutex_t*)plock;

  int ret                 = pthread_mutex_unlock(pmutex);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to unlock mutex!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_init(void** ppcv) {
  pthread_cond_t* pcv = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
  if (NULL == pcv) {
    LOGE("Error: Failed to allocate memory for condition variable!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_cond_init(pcv, NULL);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to initialize condition variable!");
    free(pcv);
    pcv = NULL;
    return UTHREAD_FAILURE;
  }

  *ppcv = (void*)pcv;
  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_deinit(void* pcv) {
  if (NULL == pcv) {
    LOGE("Error: Condition variable handle is null!");
    return UTHREAD_FAILURE;
  }

  pthread_cond_t* pcond = (pthread_cond_t*)pcv;

  int ret               = pthread_cond_destroy(pcond);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to destroy condition variable!");
    return UTHREAD_FAILURE;
  }

  free(pcond);
  pcond = NULL;
  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_wait(void* pcv, void* lock) {
  if (NULL == pcv || NULL == lock) {
    LOGE("Error: Condition variable or lock handle is null!");
    return UTHREAD_FAILURE;
  }

  pthread_cond_t*  pcond  = (pthread_cond_t*)pcv;
  pthread_mutex_t* pmutex = (pthread_mutex_t*)lock;

  int ret                 = pthread_cond_wait(pcond, pmutex);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to wait on condition variable!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_signal(void* pcv) {
  if (NULL == pcv) {
    LOGE("Error: Condition variable handle is null!");
    return UTHREAD_FAILURE;
  }

  pthread_cond_t* pcond = (pthread_cond_t*)pcv;

  int ret               = pthread_cond_signal(pcond);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to signal condition variable!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}
