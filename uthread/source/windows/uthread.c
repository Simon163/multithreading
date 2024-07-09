#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "include/uthread.h"

#define RET_SUCCESS (1)
#define RET_FAILURE (0)

typedef struct {
  HANDLE                 handle;
  DWORD                  id;
  LPTHREAD_START_ROUTINE func;
  SECURITY_ATTRIBUTES*   attr;
  void*                  arg;
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
    phandle->attr = (SECURITY_ATTRIBUTES*)pattr;
  } else {
    phandle->attr = NULL;
  }

  phandle->func = (LPTHREAD_START_ROUTINE)pfunc;
  phandle->arg  = parg;

  HANDLE hThread;
  hThread = CreateThread(NULL, 0, phandle->func, phandle->arg, 0, &phandle->id);

  if (NULL == hThread) {
    LOGE("Error: failed to create a thread!");
    free(phandle);
    phandle = NULL;
    return UTHREAD_FAILURE;
  }

  *pphandle       = (void*)phandle;
  phandle->handle = hThread;

  LOGI("The thread with ID=0x%lx is created", phandle->id);

  return UTHREAD_SUCCESS;
}

int32_t uthread_join(void* phandle) {
  if (NULL == phandle) {
    LOGE(
        "Error: thread handle is null, please create a thread properly first!");
    return UTHREAD_FAILURE;
  }

  DWORD ret = WaitForSingleObject(((thread_t*)phandle)->handle, INFINITE);
  if (WAIT_OBJECT_0 != ret) {
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

  LOGI("The thread with ID=0x%lx is about to exit", ((thread_t*)phandle)->id);

  DWORD ret = CloseHandle(((thread_t*)phandle)->handle);
  if (RET_FAILURE == ret) {
    return UTHREAD_FAILURE;
  }
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

  *pthread_id = (uint64_t)((thread_t*)phandle)->id;
  return UTHREAD_SUCCESS;
}

int32_t uthread_sleep(uint64_t microseconds) {
  Sleep((DWORD)microseconds);

  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_init(void** pplock) {
  HANDLE* plock = (HANDLE*)malloc(sizeof(HANDLE));
  if (NULL == plock) {
    LOGE("Error: failed to allocate memory!");
    return UTHREAD_FAILURE;
  }

  *plock = CreateMutex(NULL, FALSE, NULL);
  if (NULL == *plock) {
    LOGE("Error: failed to create a mutex!");
    free(plock);
    plock = NULL;
    return UTHREAD_FAILURE;
  }

  *pplock = (void*)plock;
  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_deinit(void* plock) {
  if (NULL == plock) {
    LOGE("Error: mutex handle is null!");
    return UTHREAD_FAILURE;
  }

  DWORD ret = CloseHandle((HANDLE)plock);
  if (RET_FAILURE == ret) {
    return UTHREAD_FAILURE;
  }
  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_lock(void* plock) {
  if (NULL == plock) {
    LOGE("Error: mutex handle is null!");
    return UTHREAD_FAILURE;
  }

  DWORD ret = WaitForSingleObject((HANDLE)plock, INFINITE);
  if (WAIT_OBJECT_0 != ret) {
    LOGE("Error: failed to lock the mutex!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_mutex_unlock(void* plock) {
  if (NULL == plock) {
    LOGE("Error: mutex handle is null!");
    return UTHREAD_FAILURE;
  }

  DWORD ret = ReleaseMutex((HANDLE)plock);
  if (RET_FAILURE == ret) {
    return UTHREAD_FAILURE;
  }
  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_init(void** ppcv) {
  CONDITION_VARIABLE* pcv =
      (CONDITION_VARIABLE*)malloc(sizeof(CONDITION_VARIABLE));
  if (NULL == pcv) {
    LOGE("Error: failed to allocate memory!");
    return UTHREAD_FAILURE;
  }

  InitializeConditionVariable(pcv);

  *ppcv = (void*)pcv;
  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_deinit(void* pcv) {
  if (NULL == pcv) {
    LOGE("Error: condition variable handle is null!");
    return UTHREAD_FAILURE;
  }

  free(pcv);
  pcv = NULL;

  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_wait(void* pcv, void* lock) {
  if (NULL == pcv) {
    LOGE("Error: condition variable handle is null!");
    return UTHREAD_FAILURE;
  }
  if (NULL == lock) {
    LOGE("Error: mutex handle is null!");
    return UTHREAD_FAILURE;
  }

  DWORD ret = SleepConditionVariableCS((CONDITION_VARIABLE*)pcv,
                                       (CRITICAL_SECTION*)lock, INFINITE);
  if (WAIT_OBJECT_0 != ret) {
    LOGE("Error: failed to lock the mutex!");
    return UTHREAD_FAILURE;
  }

  return UTHREAD_SUCCESS;
}

int32_t uthread_cond_signal(void* pcv) {
  if (NULL == pcv) {
    LOGE("Error: condition variable handle is null!");
    return UTHREAD_FAILURE;
  }

  WakeConditionVariable((CONDITION_VARIABLE*)pcv);
  return UTHREAD_SUCCESS;
}
