#ifndef __UTHREAD_H_
#define __UTHREAD_H_

#include <stdint.h>

/* universal logging interface, use different font colors to display different
 * types of information */
#define PRINT printf

// Error information  --- red
#define LOGE(...)                                       \
  PRINT("\033[31;22m[%s:%d] ", __FUNCTION__, __LINE__); \
  PRINT(__VA_ARGS__);                                   \
  PRINT("\033[0m\n")
// Normal information --- green
#define LOGI(...)                                       \
  PRINT("\033[32;22m[%s:%d] ", __FUNCTION__, __LINE__); \
  PRINT(__VA_ARGS__);                                   \
  PRINT("\033[0m\n")
// Debug information  --- blue
#define LOGD(...)                                       \
  PRINT("\033[34;22m[%s:%d] ", __FUNCTION__, __LINE__); \
  PRINT(__VA_ARGS__);                                   \
  PRINT("\033[0m\n")

#if defined(_MSC_VER)
#ifdef BUILDING_DLL
#define PUBLIC __declspec(dllexport)
#else
#define PUBLIC __declspec(dllimport)
#endif
#else
#define PUBLIC __attribute__((visibility("default")))
#endif

#define UTHREAD_SUCCESS (0)
#define UTHREAD_FAILURE (1)

#ifdef __cplusplus
extern "C" {
#endif

// Create a new thread
int32_t PUBLIC uthread_create(void** pphandle, void* pattr, void* pfunc,
                              void* parg);
// Wait for the thread to finish
int32_t PUBLIC uthread_join(void* phandle);
// Exit the current thread
int32_t PUBLIC uthread_close(void* phandle);
// Get the thread ID
int32_t PUBLIC uthread_id_get(void* phandle, uint64_t* thread_id);
// Sleep for specified time in microseconds
int32_t PUBLIC uthread_sleep(uint64_t microseconds);
// Initialize mutex
int32_t PUBLIC uthread_mutex_init(void** pplock);
// Deinitialize mutex
int32_t PUBLIC uthread_mutex_deinit(void* plock);
// Lock mutex
int32_t PUBLIC uthread_mutex_lock(void* plock);
// Unlock mutex
int32_t PUBLIC uthread_mutex_unlock(void* plock);
 // Initialize condition variable
int32_t PUBLIC uthread_cond_init(void** ppcv);
// Deinitialize condition variable
int32_t PUBLIC uthread_cond_deinit(void* pcv);
// Wait for condition variable
int32_t PUBLIC uthread_cond_wait(void* pcv, void* lock);
// Signal one waiting thread
int32_t PUBLIC uthread_cond_signal(void* pcv);

#ifdef __cplusplus
}
#endif

#endif  // __UTHREAD_H_
