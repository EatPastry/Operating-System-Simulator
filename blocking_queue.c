//Student: Joshua Gaynor ID: 20549366
#include "blocking_queue.h"
#include "utilities.h"
#include "list.h"
#include <semaphore.h>

void blocking_queue_terminate(BlockingQueueT* queue) {
  queue->term = 1;
}

void blocking_queue_create(BlockingQueueT** queue) {
  *queue = (BlockingQueueT*)malloc(sizeof(BlockingQueueT));
  (*queue)->front = NULL;
  (*queue)->rear = NULL;
  (*queue)->term = 0;
}

void blocking_queue_destroy(BlockingQueueT** queue) {
  unsigned int* dummy;
  while(!blocking_queue_empty(*queue)) {
    struct List* node = (*queue)->front;
    (*queue)->front = (*queue)->front->succ;

    if((*queue)->front == NULL) {
      (*queue)->rear = NULL;
    }

  free(node);
  }
  free(*queue);
  *queue = NULL;
}

void blocking_queue_push(BlockingQueueT* queue, unsigned int value) {
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

int blocking_queue_pop(BlockingQueueT* queue, unsigned int* value) {
  if(queue->term == 1) {
    return 1;
  }

  if(blocking_queue_empty(queue)) {
    //block
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

int blocking_queue_empty(BlockingQueueT* queue) {
  if(queue->front == NULL && queue->rear == NULL) {
    return 1;
  }
  return 0;
}

int blocking_queue_length(BlockingQueueT* queue) {
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
