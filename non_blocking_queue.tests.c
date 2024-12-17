//Student: Joshua Gaynor ID: 20549366
#include "non_blocking_queue.h"
#include "utilities.h"

#include <assert.h>
#include <stdio.h>

void test_create_destroy() {
  NonBlockingQueueT* queue = non_blocking_queue_create();
  assert(non_blocking_queue_empty(queue));
  assert(non_blocking_queue_length(queue) == 0);
  non_blocking_queue_destroy(queue);
}

void test_success_example() {
  // Example of a simple successful test
  NonBlockingQueueT* queue = non_blocking_queue_create();
  unsigned int* val;
  non_blocking_queue_push(queue, 10);
  non_blocking_queue_push(queue, 12);
  non_blocking_queue_push(queue, 13);
  non_blocking_queue_push(queue, 14);
  non_blocking_queue_push(queue, 15);
  int length = non_blocking_queue_length(queue);
  printf("%d\n",length);
  assert(length == 5);
  non_blocking_queue_pop(queue, &val);
  assert(val == 10);
  non_blocking_queue_pop(queue, &val);
  assert(val == 12);
  non_blocking_queue_pop(queue, &val);
  assert(val == 13);
  non_blocking_queue_pop(queue, &val);
  assert(val == 14);
  non_blocking_queue_pop(queue, &val);
  assert(val == 15);
  length = non_blocking_queue_length(queue);
  assert(length == 0);
  non_blocking_queue_destroy(queue);
}

void test_failure_example() {
  // Example of a simple failing test
  assert(1 == 0);
}

int main() {
  test_create_destroy();
  test_success_example();
  test_failure_example();
  return 0;
}
