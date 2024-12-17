//Student: Joshua Gaynor ID: 20549366
#ifndef _NON_BLOCKING_QUEUE_H_
#define _NON_BLOCKING_QUEUE_H_

#include "list.h"

typedef struct NonBlockingQueue {
  struct List* front;
  struct List* rear;
} NonBlockingQueueT;

NonBlockingQueueT* non_blocking_queue_create();
void non_blocking_queue_destroy(NonBlockingQueueT* queue);

void non_blocking_queue_push(NonBlockingQueueT* queue, unsigned int value);
int non_blocking_queue_pop(NonBlockingQueueT* queue, unsigned int* value);

int non_blocking_queue_empty(NonBlockingQueueT* queue);
int non_blocking_queue_length(NonBlockingQueueT* queue);

#endif
