//Student: Joshua Gaynor ID: 20549366
#include "simulator.h"
#include "list.h"
#include "non_blocking_queue.h"
#include "blocking_queue.h"
#include "utilities.h"
#include "logger.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

pthread_t* threads;
int* args;
ProcessIdT** pidArr;
int pCount, maxProcess;
int processCount = 0;
NonBlockingQueueT* readyQueue;

void* simulator_routine(void* arg) {
  ProcessIdT currPid;
  char buf[50];
  snprintf(buf, 50, "thread %d has started", *((int*)arg));
  //vsprintf(buf, "thread %d has started", ((int*)arg));
  logger_write(buf);
  non_blocking_queue_pop(readyQueue, &currPid);
  evaluator_evaluate(evaluator_terminates_after (currPid), 0);
  return 0;
}
void simulator_start(int thread_count, int max_processes) {
  non_blocking_queue_create(&readyQueue);
  pidArr = malloc(sizeof(ProcessIdT*) * max_processes);
  for(int i = 0; i < max_processes; i++) {
    *(pidArr + i) = NULL;
  }
  maxProcess = max_processes;
  pCount = thread_count;
  threads = malloc(sizeof(pthread_t) * thread_count);
  args = malloc(sizeof(int) * thread_count);
  
  for(int i = 0; i < thread_count; i++) {
    args[i] = i;
    if(pthread_create(threads + i, NULL, simulator_routine, args + i)) {
      printf("Creating thread %d failed\n", i);
    }
  }
}

void simulator_stop() {
  for(int i = 0; i < pCount; i++)
    pthread_join(threads[i], NULL);
  free(threads);
  free(args);
  for(int i = 0; i < maxProcess; i++) {
    free(pidArr[i]);
  }
  free(pidArr);
  non_blocking_queue_destroy(&readyQueue);
}

ProcessIdT simulator_create_process(EvaluatorCodeT const code) {
  ProcessIdT pid;
  for(int i = 0; i < maxProcess; i++) {
    if(pidArr[i] == NULL) {
      pidArr[i] = malloc(sizeof(ProcessIdT));
      pid = i;
      *pidArr[i] = pid;
      non_blocking_queue_push(readyQueue, pid);
      processCount++;
    } else {
      //block
    }
  }
  

  char buf[50];
  snprintf(buf, 50, "Process id: %d started", pid);
  logger_write(buf);
  
  return pid;
}

void simulator_wait(ProcessIdT pid) {
}

void simulator_kill(ProcessIdT pid) {
  ProcessIdT dummy;
  non_blocking_queue_pop(readyQueue, &dummy);
  for(int i = 0; i < maxProcess; i++) {
    if(*pidArr[i] == pid) {
      free(pidArr[i]);
      pidArr[i] = NULL;
      processCount--;
    }
  }
}

void simulator_event() {
}
