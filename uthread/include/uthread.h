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
PUBLIC int32_t uthread_create(void** pphandle, const void* pattr,
                              const void* pfunc, const void* parg);
// Wait for the thread to finish
PUBLIC int32_t uthread_join(const void* phandle);
// Exit the current thread
PUBLIC int32_t uthread_close(const void* phandle);
// Get the thread ID
PUBLIC int32_t uthread_id_get(const void* phandle, uint64_t* thread_id);
// Sleep for specified time in microseconds
PUBLIC int32_t uthread_sleep(uint64_t microseconds);
// Initialize mutex
PUBLIC int32_t uthread_mutex_init(void** pplock);
// Deinitialize mutex
PUBLIC int32_t uthread_mutex_deinit(const void* plock);
// Lock mutex
PUBLIC int32_t uthread_mutex_lock(const void* plock);
// Unlock mutex
PUBLIC int32_t uthread_mutex_unlock(const void* plock);
// Initialize condition variable
PUBLIC int32_t uthread_cond_init(void** ppcv);
// Deinitialize condition variable
PUBLIC int32_t uthread_cond_deinit(const void* pcv);
// Wait for condition variable
PUBLIC int32_t uthread_cond_wait(const void* pcv, const void* lock);
// Signal one waiting thread
PUBLIC int32_t uthread_cond_signal(const void* pcv);
// get the version number
PUBLIC const uint8_t* uthread_version();
#ifdef __cplusplus
}
#endif

#endif  // __UTHREAD_H_
