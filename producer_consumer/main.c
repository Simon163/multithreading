#include "include/uthread.h"
#include <stdio.h>

// mutex lock
struct uthread_mutex_t *plock;
// condition variable for producer
struct uthread_cond_t *pcv_producer;
// condition variable for consumer
struct uthread_cond_t *pcv_consumer;

// initialization
static void init(struct uthread_mutex_t **pplock,
                 struct uthread_cond_t **ppcv_producer,
                 struct uthread_cond_t **ppcv_consumer) {
  // initialization of condition variable
  uthread_cond_init(ppcv_producer);
  uthread_cond_init(ppcv_consumer);

  // initialization of mutex lock
  uthread_mutex_init(pplock);
}

// deinitialization
static void deinit(struct uthread_mutex_t *plock,
                   struct uthread_cond_t *pcv_producer,
                   struct uthread_cond_t *pcv_consumer) {
  // deinitialization of condition variable
  uthread_cond_deinit(pcv_producer);
  uthread_cond_deinit(pcv_consumer);

  // deinitialization of mutex lock
  uthread_mutex_deinit(plock);
}

// producer thread, produce one item each time
void *handle_product(void *arg) {
  int i;
  int *product = NULL;
  product = (int *)arg;
  for (i = 1; i < 50; i++) {
    printf("Producer thread, the %d-th round\n", i);
    // lock the mutex so the consumer can not consume the item when producing
    uthread_mutex_lock(plock);
    // the warehouse is full, the producer thread is blocked and keep waiting
    // for the consumer thread to consume the produced items
    if (*product >= 4) {
      printf("\033[31;22mWarehouse is full and production is "
             "suspended...\033[0m\n");
      uthread_cond_wait(pcv_producer, plock);
    }
    printf("Producing....\n");
    uthread_sleep(2000);
    printf("Procuce done ....\n");
    *product += 1;
    // notify the consumer thread
    uthread_cond_signal(pcv_consumer);
    printf("\033[32;22mProduced one item, %d times in total, "
           "there are %d items left in the warehouse\033[0m\n",
           i, *product);
    printf("Send the signal to the consumer, produce done \n");
    // unlock the mutex so consumer can consume items
    uthread_mutex_unlock(plock);
    uthread_sleep(50);
  }
  return NULL;
}

// consumer thread, consume two items each time
void *handle_consume(void *arg) {
  int i;
  int *product = NULL;
  product = (int *)arg;
  for (i = 1; i < 50; i++) {
    printf("Consumer thread, the %d-th round\n", i);
    // lock the mutex so the producer can not produce the item when consuming
    uthread_mutex_lock(plock);
    // if the number of products in the warehouse is less than 2, the consumer
    // thread will be blocked
    if (*product <= 1) {
      printf("\033[31;22mOut of stock, please wait...\033[0m\n");
      uthread_cond_wait(pcv_consumer, plock);
    }
    // consume two items once
    printf("Consuming...\n");
    uthread_sleep(2000);
    *product -= 2;
    printf("Consume done...\n");
    printf("\033[34;22mConsumed two items, %d times in total, there are %d "
           "left in the warehouse\033[0m\n",
           i, *product);
    uthread_cond_signal(pcv_producer);
    printf("Send the signal to the producer, consume done \n");
    // unlock the mutex so producer can produce items
    uthread_mutex_unlock(plock);
    uthread_sleep(30);
    // take a break for every six times consumption
    if (i % 6 == 0) {
      uthread_sleep(9000);
    }
  }
  return NULL;
}

int main() {
  // handle of producer
  struct uthread_t *phandle_producer = NULL;
  // handle of consumer
  struct uthread_t *phandle_consumer = NULL;

  int ret;
  int items = 3;
  printf("There are %d items in the warehouse at first\n", items);
  // initializaton of condition varaiables and mutex lock
  init(&plock, &pcv_producer, &pcv_consumer);
  // create a thread for producer
  ret = uthread_create(&phandle_producer, NULL, handle_product, &items);
  if (ret != 0) {
    printf("Failed to create the producer thread\n");
    return -1;
  }
  // create a thread for consumer
  ret = uthread_create(&phandle_consumer, NULL, handle_consume, &items);
  if (ret != 0) {
    printf("Failed to create the consumer thread\n");
    return -1;
  }

  // waiting for the producer thread to finish its work
  uthread_join(phandle_producer);
  // waiting for the consumer thread to finish its work
  uthread_join(phandle_consumer);

  // deinitializaton of condition varaiables and mutex lock
  deinit(plock, pcv_producer, pcv_consumer);

  // close the handles of threads
  uthread_close(phandle_producer);
  uthread_close(phandle_consumer);

  return 0;
}