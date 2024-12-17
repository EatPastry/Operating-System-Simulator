//Student: Joshua Gaynor ID: 20549366
#include "non_blocking_queue.h"
#include "utilities.h"
#include "list.h"
#include <assert.h>

void non_blocking_queue_create(NonBlockingQueueT* queue) {
  queue = (NonBlockingQueueT*)malloc(sizeof(NonBlockingQueueT));
  queue->front = queue->rear = NULL;
}

void non_blocking_queue_destroy(NonBlockingQueueT* queue) {
  free(queue);
}

void non_blocking_queue_push(NonBlockingQueueT* queue, unsigned int value) {
  assert(queue);
  struct List* node = alloc_node();
  node->value = value;
  if(queue->rear == NULL) {
    queue->front = queue->rear = node;
    return;
  }

  queue->rear->succ = node;
  queue->rear = node;
}

int non_blocking_queue_pop(NonBlockingQueueT* queue, unsigned int* value) {
  if(non_blocking_queue_empty(queue)) {
    return 1;
  }
  struct List* node = queue->front;
  queue->front = queue->front->succ;

  if(queue->front == NULL) {
    queue->rear == NULL;
  }
  free_node(node);
  return 0;
}

int non_blocking_queue_empty(NonBlockingQueueT* queue) {
  if(queue->front == NULL && queue->rear == NULL) {
    return 1;
  }
  return 0;
}

int non_blocking_queue_length(NonBlockingQueueT* queue) {
  return 0;
}
