#include "simulator.h"
#include "list.h"
#include "non_blocking_queue.h"
#include "blocking_queue.h"
#include "utilities.h"
#include "logger.h"
#include <pthread.h>
#include <stdio.h>

void simulator_routine(int* arg) {
  printf("thread %d has started\n", *((int*)arg));
}
void simulator_start(int thread_count, int max_processes) {
  pthread_t threads[thread_count];
  int *args = malloc(sizeof(int) * thread_count);
  for(int i = 0; i < thread_count; i++) {
    args[i] = i;
    if(pthread_create(threads + i, NULL, simulator_routine, args + i)) {
      printf("Creating thread %d failed\n", i);
    }
  }
  for(int i = 0; i < thread_count; i++)
    pthread_join(threads[i], NULL);
  free(args);
}

void simulator_stop() {
}

ProcessIdT simulator_create_process(EvaluatorCodeT const code) {
  ProcessIdT pid = 0;
  return pid;
}

void simulator_wait(ProcessIdT pid) {
}

void simulator_kill(ProcessIdT pid) {
}

void simulator_event() {
}
