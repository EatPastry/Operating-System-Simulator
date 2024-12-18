//Student: Joshua Gaynor ID: 20549366
#include "non_blocking_queue.h"
#include "utilities.h"
#include "list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void non_blocking_queue_create(NonBlockingQueueT** queue) {
  *queue = (NonBlockingQueueT*)malloc(sizeof(NonBlockingQueueT));
  (*queue)->front = NULL;
  (*queue)->rear = NULL;
}

/*
NonBlockingQueueT* non_blocking_queue_create() {
  NonBlockingQueueT* queue = (NonBlockingQueueT*)malloc(sizeof(NonBlockingQueueT));
  queue->front = NULL;
  queue->rear = NULL;
  return queue;
}
*/

void non_blocking_queue_destroy(NonBlockingQueueT* queue) {
  unsigned int* dummy;
  while(!non_blocking_queue_empty(queue)) {
    non_blocking_queue_pop(queue, dummy);
  }
  free(queue);
}

void non_blocking_queue_push(NonBlockingQueueT* queue, unsigned int value) {
  assert(queue);
  struct List* node = (struct List*)malloc(sizeof(struct List));

  node->value = value;
  node->pred = NULL;
  node->succ = NULL;

  if(queue->rear == NULL) {
    queue->front = node;
    queue->rear = node;
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
    queue->rear = NULL;
  }

  *value = node->value;
  free(node);
  return 0;
}

int non_blocking_queue_empty(NonBlockingQueueT* queue) {
  if(queue->front == NULL && queue->rear == NULL) {
    return 1;
  }
  return 0;
}

int non_blocking_queue_length(NonBlockingQueueT* queue) {
  int i;
  struct List* currNode = queue->front;
  if(queue->front == NULL) {
    return 0;
  }
  for(i = 0; currNode != queue->rear; i++) {
    currNode = currNode->succ;
  }
  i++;
  return i;
}
