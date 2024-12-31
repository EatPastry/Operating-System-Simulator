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
#include <unistd.h>

typedef struct {
    ProcessIdT pid;
    EvaluatorCodeT code; // Function pointer to the code the process will run
    ProcessStateT state;
    unsigned int last_PC;      // Process state, e.g., "READY", "RUNNING"
} ProcessControlBlockT;

pthread_t* threads;
int* args;
ProcessControlBlockT** process_table = NULL;
int pCount, maxProcess;
int processCount = 0;
int processFinished = 0;
int run = 1;
NonBlockingQueueT* readyQueue; 
NonBlockingQueueT* eventQueue;

pthread_cond_t qu_empty_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t qu_full_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t wait_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t table_condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t qu_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t event_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t table_mutex = PTHREAD_MUTEX_INITIALIZER;

void* simulator_routine(void* arg) {
  ProcessIdT currPid;
  EvaluatorResultT result;
  unsigned int convertedPid;
  char buf[50];
  pthread_mutex_lock(&buffer_mutex);
  snprintf(buf, 50, "Thread %d has started", *((int*)arg));
  logger_write(buf);
  pthread_mutex_unlock(&buffer_mutex);

    while (run) {
        usleep(0.2);
        //usleep(0.1);
        if (non_blocking_queue_empty(readyQueue)) {
        } else {
            //printf("No processes in the ready queue. Thread %d is idle.\n", *((int *)arg));
          pthread_mutex_lock(&queue_mutex);

          if (!non_blocking_queue_empty(readyQueue)) {
          non_blocking_queue_pop(readyQueue, &currPid);
          }
          pthread_mutex_unlock(&queue_mutex);
          
          pthread_mutex_lock(&buffer_mutex);
          snprintf(buf, 50, "Thread %d is running process %d", *((int *)arg), currPid);
          logger_write(buf);
          pthread_mutex_unlock(&buffer_mutex);

          
          pthread_mutex_lock(&table_mutex);
          if (process_table[currPid] != NULL) {
            (*process_table[currPid]).state = running;
          }
          pthread_mutex_unlock(&table_mutex);

          result = evaluator_evaluate((*process_table[currPid]).code, (*process_table[currPid]).last_PC);
          

          pthread_mutex_lock(&table_mutex);
          if (process_table[currPid] != NULL) {
            (*process_table[currPid]).last_PC = result.PC;
            
            if(result.reason == reason_blocked) {
              (*process_table[currPid]).state = blocked;
              pthread_mutex_lock(&event_queue_mutex);
              non_blocking_queue_push(eventQueue, currPid);
              pthread_mutex_unlock(&event_queue_mutex);

            } else if(result.reason == reason_terminated) {
              (*process_table[currPid]).state = terminated;
              pthread_mutex_unlock(&table_mutex);

            } else if(result.reason == reason_timeslice_ended) {
              (*process_table[currPid]).state = ready;

              pthread_mutex_lock(&buffer_mutex);
              snprintf(buf, 50, "Process %d timeslice ended, re-queuing", currPid);
              logger_write(buf);
              pthread_mutex_unlock(&buffer_mutex);

              pthread_mutex_lock(&queue_mutex);
              non_blocking_queue_push(readyQueue, currPid);
              pthread_mutex_unlock(&queue_mutex);

              pthread_mutex_unlock(&table_mutex);

              pthread_mutex_unlock(&wait_mutex);

            }
          }
          /*
          if(result.PC >= 5) {

            if (process_table[currPid] != NULL) {
              (*process_table[currPid]).state = terminated;
            }

            pthread_mutex_unlock(&table_mutex);
            
            processFinished = 1;
            //pthread_cond_signal(&wait_cond);
            //pthread_mutex_unlock(&wait_mutex);
          } else {
            //pthread_mutex_lock(&table_mutex);
            if (process_table[currPid] != NULL) {
              (*process_table[currPid]).state = ready;
            }
            
          //pthread_cond_signal(&table_condition);
         }*/
        } 
        pthread_cond_signal(&table_condition);
            
    }
  return 0;
}

void simulator_start(int thread_count, int max_processes) {
  non_blocking_queue_create(&readyQueue);
  non_blocking_queue_create(&eventQueue);

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
  run = 0;
  for(int i = 0; i < pCount; i++)
    pthread_join(threads[i], NULL);
  free(threads);
  free(args);
  for(int i = 0; i < maxProcess; i++) {
    free(process_table[i]);
  }
  free(process_table);
  non_blocking_queue_destroy(&readyQueue);
  non_blocking_queue_destroy(&eventQueue);
}

ProcessIdT simulator_create_process(EvaluatorCodeT const code) {
  ProcessIdT pid;
  while(run) {
  for(int i = 0; i < maxProcess; i++) {
    if(process_table[i] == NULL) {
      pthread_mutex_lock(&table_mutex);
      process_table[i] = malloc(sizeof(ProcessControlBlockT));
      pthread_mutex_unlock(&table_mutex);
      
      pid = i;
      processCount++;

      ProcessControlBlockT pcb;
      pcb.pid = pid;
      pcb.code = code;
      pcb.state = ready;
      pcb.last_PC = 0;

      pthread_mutex_lock(&table_mutex);
      *process_table[pid] = pcb;
      pthread_mutex_unlock(&table_mutex);

      pthread_mutex_lock(&queue_mutex);
      non_blocking_queue_push(readyQueue, pid);
      pthread_mutex_unlock(&queue_mutex);

      char buf[50];
      pthread_mutex_lock(&buffer_mutex);
      snprintf(buf, 50, "Process id: %d created", pid);
      logger_write(buf);
      pthread_mutex_unlock(&buffer_mutex);
      return pid;
    }
  }

  pthread_cond_wait(&qu_empty_cond, &qu_mutex);

  }
  pthread_mutex_unlock(&qu_mutex);
  return -1; 
}

ProcessIdT simulator_request_pid() {
  ProcessIdT pid;
  
  return pid;
}

void simulator_wait(ProcessIdT pid) {
  int found = 0;
  int i;
  char buf[50];
  pthread_mutex_lock(&buffer_mutex);
  snprintf(buf, 50, "Waiting for process id: %d", pid);
  logger_write(buf);
  pthread_mutex_unlock(&buffer_mutex);
   
  while(run) {
  //pthread_mutex_lock(&wait_mutex);
  //while(processFinished == 0) 
  //  pthread_cond_wait(&wait_cond, &wait_mutex);
  //pthread_mutex_lock(&wait_mutex);
  //pthread_cond_wait(&wait_cond,&wait_mutex);
  //pthread_mutex_unlock(&wait_mutex);
  
  found = 0;
  pthread_mutex_lock(&table_mutex);
  for(i = 0; i < maxProcess; i++) {
    if(process_table[i] != NULL && (*process_table[i]).pid == pid) {
      
      found = 1;
      //usleep(3);
      if((*process_table[i]).state == terminated) {
        
       // pthread_cond_wait(&table_condition, &table_mutex);
        pthread_mutex_lock(&buffer_mutex);
        snprintf(buf, 50, "Process id: %d finished", pid);
        logger_write(buf);
        pthread_mutex_unlock(&buffer_mutex);

        //printf("stoppped %d\n", process_table[i]);
        if (process_table[i] != NULL) {
          free(process_table[i]);
          process_table[i] = NULL;
        }
        pthread_mutex_unlock(&table_mutex);
        //pthread_cond_signal(&wait_cond);
        processFinished = 0;
        //pthread_mutex_unlock(&wait_mutex);
        //pthread_cond_signal(&wait_cond);
        
        return;
      }
      
    }

  }
  pthread_mutex_unlock(&table_mutex);

  }
}

void simulator_kill(ProcessIdT pid) {
  /*ProcessIdT dummy;
  non_blocking_queue_pop(readyQueue, &dummy);
  for(int i = 0; i < maxProcess; i++) {
    if((*process_table[i]).pid == pid) {
      free(process_table[i]);
      process_table[i] = NULL;
      processCount--;
    }
  }*/
}

void simulator_event() {
  if(!non_blocking_queue_empty(eventQueue)) {
    ProcessIdT currPid;
    char buf[50];
    non_blocking_queue_pop(eventQueue, &currPid);
    non_blocking_queue_push(readyQueue, currPid);
    pthread_mutex_lock(&buffer_mutex);
    snprintf(buf, 50, "Process id: %d moved to ready queue", currPid);
    logger_write(buf);
    pthread_mutex_unlock(&buffer_mutex);
  }
}
