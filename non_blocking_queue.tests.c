#include "non_blocking_queue.h"
#include "utilities.h"

#include <assert.h>
#include <stdio.h>

void test_success_example() {
  // Example of a simple successful test
  NonBlockingQueueT* queue;
  unsigned int* val;
  non_blocking_queue_create(queue);
  non_blocking_queue_push(queue, 2);
  //non_blocking_queue_pop(queue, val);
  //printf("%s\n", val);
  //assert(val == val);
  //non_blocking_queue_destroy(queue);
}

void test_failure_example() {
  // Example of a simple failing test
  assert(1 == 0);
}

int main() {
  test_success_example();
  test_failure_example();
  return 0;
}
