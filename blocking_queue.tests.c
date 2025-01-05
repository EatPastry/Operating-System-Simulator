//Student: Joshua Gaynor ID: 20549366
#include "blocking_queue.h"
#include "utilities.h"

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>

pthread_t* test_threads;
BlockingQueueT* public_queue;
pthread_mutex_t public_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void test_create_destroy() {
  // Creates queue, checks queue attributes are correct, destroys
  BlockingQueueT* queue;
  blocking_queue_create(&queue);
  assert(blocking_queue_empty(queue) == 1);
  assert(blocking_queue_length(queue) == 0);
  blocking_queue_destroy(&queue);
}

void test_push_term_pop() {
  //Creates queue, pushes a value, terminates the queue, attempts to pop from queue, popping should be unsuccessful
  BlockingQueueT* queue;
  blocking_queue_create(&queue);
  unsigned int dummy;
  blocking_queue_push(queue, 10);
  blocking_queue_terminate(queue);
  assert(blocking_queue_pop(queue, &dummy) == 1);
  blocking_queue_destroy(&queue);
}

void test_push_pop() {
  //Create queue, push a value, pop the value, check the value is correct
  BlockingQueueT* queue;
  blocking_queue_create(&queue);
  unsigned int dummy;
  blocking_queue_push(queue, 10);
  blocking_queue_pop(queue, &dummy);
  assert(dummy == 10);
  blocking_queue_destroy(&queue);
}

void* thread_test(void* arg) {
  //check which thread is running, check value depending on thread
    unsigned int val;
    if(*((int*)arg) == 1) {
      usleep(0.01);
    }
    pthread_mutex_lock(&public_queue_mutex);
    blocking_queue_pop(public_queue, &val);
    pthread_mutex_unlock(&public_queue_mutex);

    if(*((int*)arg) == 0) {
      //printf("%d\n", val);
      assert(val == 5);
    } else {
      //printf("%d\n", val);
      //assert(val == 10);
    }
    
    
}

void test_create_popempty() {
  //Create threads, only push one value to the queue, make the other thread block until it recieves a value to pop
    int args[2];
    args[0] = 0;
    args[1] = 1;
    test_threads = malloc(sizeof(pthread_t) * 2);
    blocking_queue_create(&public_queue);
    blocking_queue_push(public_queue, 5);
    if(pthread_create(test_threads, NULL, thread_test, args + 0)) {
        printf("Creating thread failed\n");
    }
    
    if(pthread_create(test_threads + 1, NULL, thread_test, args + 1)) {
        printf("Creating thread failed\n");
    }
    //
    pthread_mutex_lock(&public_queue_mutex);
    blocking_queue_push(public_queue, 10);
    pthread_mutex_unlock(&public_queue_mutex);
}

int main() {
  test_create_destroy();
  test_push_term_pop();
  test_push_pop();
  test_create_popempty();
  pthread_join(test_threads[0], NULL);
  pthread_join(test_threads[1], NULL);
  free(test_threads);
  blocking_queue_destroy(&public_queue);
  return 0;
}
