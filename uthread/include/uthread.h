#ifndef __UTHREAD_H_
#define __UTHREAD_H_

#include <stdint.h>
#include <stdio.h>

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

struct uthread_t;
struct uthread_mutex_t;
struct uthread_cond_t;

// Create a new thread
PUBLIC int32_t uthread_create(struct uthread_t** pphandle, const void* pattr,
                              const void* pfunc, const void* parg);
// Wait for the thread to finish
PUBLIC int32_t uthread_join(const struct uthread_t* phandle);
// Exit the current thread
PUBLIC int32_t uthread_close(const struct uthread_t* phandle);
// Get the thread ID
PUBLIC int32_t uthread_id_get(const struct uthread_t* phandle,
                              uint64_t*               thread_id);
// Sleep for specified time in microseconds
PUBLIC int32_t uthread_sleep(uint64_t microseconds);
// Initialize mutex
PUBLIC int32_t uthread_mutex_init(struct uthread_mutex_t** ppmutex);
// Deinitialize mutex
PUBLIC int32_t uthread_mutex_deinit(const struct uthread_mutex_t* pmutex);
// Lock mutex
PUBLIC int32_t uthread_mutex_lock(const struct uthread_mutex_t* pmutex);
// Unlock mutex
PUBLIC int32_t uthread_mutex_unlock(const struct uthread_mutex_t* pmutex);
// Initialize condition variable
PUBLIC int32_t uthread_cond_init(struct uthread_cond_t** ppcond);
// Deinitialize condition variable
PUBLIC int32_t uthread_cond_deinit(const struct uthread_cond_t* pcond);
// Wait for condition variable
PUBLIC int32_t uthread_cond_wait(const struct uthread_cond_t*  pcond,
                                 const struct uthread_mutex_t* pmutex);
// Signal one waiting thread
PUBLIC int32_t uthread_cond_signal(const struct uthread_cond_t* pcond);
// get the version number
PUBLIC const uint8_t* uthread_version();
#ifdef __cplusplus
}
#endif

#endif  // __UTHREAD_H_
