//Student: Joshua Gaynor ID: 20549366
#include "non_blocking_queue.h"
#include "utilities.h"

#include <assert.h>
#include <stdio.h>

void test_create_destroy() {
  NonBlockingQueueT* queue;
  non_blocking_queue_create(&queue);
  
  assert(non_blocking_queue_empty(queue) == 1);
  assert(non_blocking_queue_length(queue) == 0);
  non_blocking_queue_destroy(&queue);
}

void test_pushfive_popfive() {
  //Create queue, push five values, check queue length, pop them all, destroy queue
  NonBlockingQueueT* queue;
  non_blocking_queue_create(&queue);
  unsigned int val;

  non_blocking_queue_push(queue, 10);
  non_blocking_queue_push(queue, 12);
  non_blocking_queue_push(queue, 13);
  non_blocking_queue_push(queue, 14);
  non_blocking_queue_push(queue, 15);
  int length = non_blocking_queue_length(queue);
  assert(length == 5);
  non_blocking_queue_pop(queue, &val);
  assert((int)val == 10);
  non_blocking_queue_pop(queue, &val);
  assert((int)val == 12);
  non_blocking_queue_pop(queue, &val);
  assert((int)val == 13);
  non_blocking_queue_pop(queue, &val);
  assert((int)val == 14);
  non_blocking_queue_pop(queue, &val);
  assert((int)val == 15);
  length = non_blocking_queue_length(queue);
  assert(length == 0);
  non_blocking_queue_destroy(&queue);
}

void test_pushfive_popthree() {
  //Create queue, push five values, check queue length, pop only three, destroy queue.
  NonBlockingQueueT* queue;
  non_blocking_queue_create(&queue);
  unsigned int val;

  non_blocking_queue_push(queue, 10);
  non_blocking_queue_push(queue, 12);
  non_blocking_queue_push(queue, 13);
  non_blocking_queue_push(queue, 14);
  non_blocking_queue_push(queue, 15);

  int length = non_blocking_queue_length(queue);
  assert(length == 5);

  non_blocking_queue_pop(queue, &val);
  assert((unsigned int)val == 10);
  non_blocking_queue_pop(queue, &val);
  assert((unsigned int)val == 12);
  non_blocking_queue_pop(queue, &val);
  assert((unsigned int)val == 13);
  non_blocking_queue_destroy(&queue);
}

void test_pop_empty() {
  //Create queue, check the length = 0, check popping was unsuccessful, destroy queue
  NonBlockingQueueT* queue;
  non_blocking_queue_create(&queue);
  unsigned int val;
  assert(non_blocking_queue_length(queue) == 0);
  assert(non_blocking_queue_pop(queue, &val) == 1);
  non_blocking_queue_destroy(&queue);
}

int main() {
  test_create_destroy();
  test_pushfive_popfive();
  test_pushfive_popthree();
  test_pop_empty();
  return 0;
}
