#include "blocking_queue.h"
#include "utilities.h"

#include <assert.h>

void test_create_destroy() {
  // Example of a simple successful test
  BlockingQueueT* queue;
  blocking_queue_create(&queue);
  assert(blocking_queue_empty(queue) == 1);
  assert(blocking_queue_length(queue) == 0);
  blocking_queue_destroy(queue);
}

void test_push_term_pop() {
  BlockingQueueT* queue;
  blocking_queue_create(&queue);
  unsigned int* dummy;
  blocking_queue_push(queue, 10);
  blocking_queue_terminate(queue);
  assert(blocking_queue_pop(queue, &dummy) == 1);
  blocking_queue_destroy(queue);
}

/*void test_create_popempty() {
  BlockingQueueT* queue;
  unsigned int* val;
  blocking_queue_create(&queue);
  blocking_queue_pop(queue, &val);
  
  blocking_queue_destroy(queue);
}*/

int main() {
  test_create_destroy();
  test_push_term_pop();
  //test_create_popempty();
  return 0;
}
