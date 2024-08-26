#include <stdint.h>
#include <stdio.h>

#include "include/uthread.h"

#if __linux__ || __linux || linux
#include <pthread.h>
#elif _WIN32 || _WIN64
#include <windows.h>
#else
#error "Unsupported platform!"
#endif

void* ThreadFunc(void* pParam) {
  int* p = (int*)pParam;

#if __linux__ || __linux || linux
  unsigned long id = pthread_self();
#elif _WIN32 || _WIN64
  DWORD id = GetCurrentThreadId();
#else
#error "Unsupported platform!"
#endif

  LOGI("The thread with ID= 0x%lx is running with parameter: %d", id, *p);

  // uthread_sleep 300ms to simulate some work in the thread
  uthread_sleep(300);

  return NULL;
}

int main() {
  int               ret       = 0;
  struct uthread_t* phandle   = NULL;
  uint64_t          thread_id = 0;

  {
    const uint8_t* version = uthread_version();
    LOGI("Uthread version is: %s", version);
  }

  int param = 123;
  {
    ret = uthread_create(&phandle, NULL, (void*)ThreadFunc, (void*)&param);
    if (ret) {
      LOGE("Thread creation failed");
      return UTHREAD_FAILURE;
    }

    ret = uthread_id_get(phandle, &thread_id);
    if (ret) {
      LOGE("Thread ID get failed");
      return UTHREAD_FAILURE;
    }
    LOGI("The thread with ID=0x%lx is running", thread_id);

    ret = uthread_join(phandle);
    if (ret) {
      LOGE("Thread join failed");
      return UTHREAD_FAILURE;
    }

    uthread_close(phandle);
  }

  return UTHREAD_SUCCESS;
}
