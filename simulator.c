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

typedef struct {
    ProcessIdT pid;
    EvaluatorCodeT code; // Function pointer to the code the process will run
    ProcessStateT state;      // Process state, e.g., "READY", "RUNNING"
} ProcessControlBlockT;

pthread_t* threads;
int* args;
ProcessControlBlockT** process_table = NULL;
int pCount, maxProcess;
int processCount = 0;
NonBlockingQueueT* readyQueue;

void* simulator_routine(void* arg) {
  ProcessIdT currPid;
  unsigned int convertedPid;
  char buf[50];
  snprintf(buf, 50, "thread %d has started", *((int*)arg));
  //vsprintf(buf, "thread %d has started", ((int*)arg));
  logger_write(buf);
      while (1) {
        // Check if there are processes in the ready queue
        if (!non_blocking_queue_pop(readyQueue, &currPid)) {
            printf("No processes in the ready queue. Thread %d is idle.\n", *((int *)arg));
            sleep(1); // Simulate idle waiting
            continue;
        }

        // Log process start
        snprintf(buf, 50, "Thread %d is running process %d", *((int *)arg), currPid);
        logger_write(buf);

        // Evaluate the process
        evaluator_evaluate((*process_table[currPid]).code, 1);
        convertedPid = currPid;
        if (evaluator_terminates_after(convertedPid)) {
            snprintf(buf, 50, "Process %d has terminated", currPid);
            logger_write(buf);
            // Process is done, no further action needed
        } else {
            snprintf(buf, 50, "Process %d timeslice ended, re-queuing", currPid);
            logger_write(buf);
            // Re-queue the process for the next round
            non_blocking_queue_push(readyQueue, currPid);
        }
    }
  //non_blocking_queue_pop(readyQueue, &currPid);
  //evaluator_evaluate(evaluator_terminates_after (currPid), 0);
  return 0;
}
void simulator_start(int thread_count, int max_processes) {
  non_blocking_queue_create(&readyQueue);

  process_table = malloc(sizeof(ProcessControlBlockT*) * max_processes);

  for(int i = 0; i < max_processes; i++) {
    *(process_table + i) = NULL;
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
    free(process_table[i]);
  }
  free(process_table);
  non_blocking_queue_destroy(&readyQueue);
}

ProcessIdT simulator_create_process(EvaluatorCodeT const code) {
  ProcessIdT pid;
  for(int i = 0; i < maxProcess; i++) {
    if(process_table[i] == NULL) {
      process_table[i] = malloc(sizeof(ProcessControlBlockT));
      pid = i;
      processCount++;
      break;
    } else {
      //block
    }
  }

  ProcessControlBlockT pcb;
  pcb.pid = pid;
  pcb.code = code;
  pcb.state = ready;

  *process_table[pid] = pcb;

  non_blocking_queue_push(readyQueue, pid);

  char buf[50];
  snprintf(buf, 50, "Process id: %d created", pid);
  logger_write(buf);
  
  return pid;
}

ProcessIdT simulator_request_pid() {
  ProcessIdT pid;
  
  return pid;
}

void simulator_wait(ProcessIdT pid) {
}

void simulator_kill(ProcessIdT pid) {
  ProcessIdT dummy;
  non_blocking_queue_pop(readyQueue, &dummy);
  for(int i = 0; i < maxProcess; i++) {
    if((*process_table[i]).pid == pid) {
      free(process_table[i]);
      process_table[i] = NULL;
      processCount--;
    }
  }
}

void simulator_event() {
}
