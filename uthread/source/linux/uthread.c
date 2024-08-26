#include "include/uthread.h"

#include <pthread.h>
#include <stdint.h>
#include <time.h>

#define RET_SUCCESS (0)
#define RET_FAILURE (1)

typedef void* (*start_routine)(void*);

struct uthread_t {
  pthread_t       handle;
  pthread_t       id;
  start_routine   func;
  pthread_attr_t* attr;
  void*           arg;
};

struct uthread_mutex_t {
  pthread_mutex_t lock;
};

struct uthread_cond_t {
  pthread_cond_t cv;
};

int32_t uthread_create(struct uthread_t** pphandle, const void* pattr,
                       const void* pfunc, const void* parg) {
  if (NULL == pfunc) {
    LOGE("Error: thread function is not specified!");
    return UTHREAD_FAILURE;
  }
  struct uthread_t* phandle =
      (struct uthread_t*)malloc(sizeof(struct uthread_t));
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

  LOGI("The thread with ID=0x%lx is created",
       ((struct uthread_t*)(*pphandle))->handle);

  return UTHREAD_SUCCESS;
}

int32_t uthread_join(const struct uthread_t* phandle) {
  if (NULL == phandle) {
    LOGE(
        "Error: thread handle is null, please create a thread properly first!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_join(((struct uthread_t*)phandle)->handle, NULL);

  if (RET_SUCCESS != ret) {
    LOGE("Error: failed to join the thread!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_close(const struct uthread_t* phandle) {
  if (NULL == phandle) {
    LOGE(
        "Error: thread handle is null, please create a thread properly first!");
    return UTHREAD_FAILURE;
  }

  // store the handle for the purpose of invoking exiting function
  pthread_t handle = ((struct uthread_t*)phandle)->handle;
  LOGI("The thread with ID=0x%lx is about to exit",
       ((struct uthread_t*)phandle)->id);

  // dellocate memory
  free(phandle);

  // invoke exiting function of the thread
  pthread_exit(&handle);

  return UTHREAD_SUCCESS;
}

int32_t uthread_id_get(const struct uthread_t* phandle, uint64_t* pthread_id) {
  if (NULL == phandle) {
    LOGE(
        "Error: thread handle is null, please create a thread properly first!");
    return UTHREAD_FAILURE;
  }
  if (NULL == pthread_id) {
    LOGE("Error: please pass a valid thread id pointer!");
    return UTHREAD_FAILURE;
  }

  *pthread_id = (uint64_t)((struct uthread_t*)phandle)->handle;

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

int32_t uthread_mutex_init(struct uthread_mutex_t** ppmutex) {
  if (NULL == ppmutex) {
    LOGE("Error: Mutex pointer is null!");
    return UTHREAD_FAILURE;
  }

  LOGI("sizeof(uthread_mutex_t): %d", sizeof(struct uthread_mutex_t));
  struct uthread_mutex_t* pmutex =
      (struct uthread_mutex_t*)malloc(sizeof(struct uthread_mutex_t));
  if (NULL == pmutex) {
    LOGE("Error: Failed to allocate memory for mutex!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_mutex_init(&pmutex->lock, NULL);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to initialize mutex!");
    free(pmutex);
    pmutex = NULL;
    return UTHREAD_FAILURE;
  }

  *ppmutex = pmutex;
  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_deinit(const struct uthread_mutex_t* pmutex) {
  if (NULL == pmutex) {
    LOGE("Error: Mutex pointer is null!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_mutex_destroy(&pmutex->lock);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to destroy mutex!");
    return UTHREAD_FAILURE;
  }

  free(pmutex);
  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_lock(const struct uthread_mutex_t* pmutex) {
  if (NULL == pmutex) {
    LOGE("Error: Mutex pointer is null!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_mutex_lock(&pmutex->lock);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to lock mutex!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_unlock(const struct uthread_mutex_t* pmutex) {
  if (NULL == pmutex) {
    LOGE("Error: Mutex pointer is null!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_mutex_unlock(&pmutex->lock);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to unlock mutex!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_init(struct uthread_cond_t** ppcond) {
  if (NULL == ppcond) {
    LOGE("Error: Condition variable pointer is null!");
    return UTHREAD_FAILURE;
  }
  struct uthread_cond_t* pcv =
      (struct uthread_cond_t*)malloc(sizeof(struct uthread_cond_t));
  if (NULL == pcv) {
    LOGE("Error: Failed to allocate memory for condition variable!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_cond_init(&pcv->cv, NULL);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to initialize condition variable!");
    free(pcv);
    pcv = NULL;
    return UTHREAD_FAILURE;
  }

  *ppcond = pcv;
  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_deinit(const struct uthread_cond_t* pcond) {
  if (NULL == pcond) {
    LOGE("Error: Condition variable pointer is null!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_cond_destroy(&pcond->cv);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to destroy condition variable!");
    return UTHREAD_FAILURE;
  }

  free(pcond);
  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_wait(const struct uthread_cond_t*  pcond,
                          const struct uthread_mutex_t* pmutex) {
  if (NULL == pcond || NULL == pmutex) {
    LOGE("Error: Condition variable or lock handle is null!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_cond_wait(&pcond->cv, &pmutex->lock);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to wait on condition variable!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_signal(const struct uthread_cond_t* pcond) {
  if (NULL == pcond) {
    LOGE("Error: Condition variable pointer is null!");
    return UTHREAD_FAILURE;
  }

  int ret = pthread_cond_signal(&pcond->cv);

  if (RET_SUCCESS != ret) {
    LOGE("Error: Failed to signal condition variable!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

const uint8_t* uthreadVersion = UTHREAD_VERSION;

const uint8_t* uthread_version() { return uthreadVersion; }
