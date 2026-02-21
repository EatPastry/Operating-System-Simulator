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
int process_terminate = 0;
int table_access = 0;
int run = 1;
NonBlockingQueueT* readyQueue; 
NonBlockingQueueT* eventQueue;

pthread_cond_t qu_empty_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t qu_full_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t wait_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t term_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t table_condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t table_access_cond = PTHREAD_COND_INITIALIZER;
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
      //usleep(0.2);
        //usleep(0.1);
      if (non_blocking_queue_empty(readyQueue)) {
      
      } else {
        pthread_mutex_lock(&queue_mutex);
          non_blocking_queue_pop(readyQueue, &currPid);
        pthread_mutex_unlock(&queue_mutex);
        if(process_table[currPid] != NULL){
          if((*process_table[currPid]).state == ready) {

          pthread_mutex_lock(&buffer_mutex);
            snprintf(buf, 50, "Thread %d is running process %d", *((int *)arg), currPid);
            logger_write(buf);
          pthread_mutex_unlock(&buffer_mutex);

          if(process_table[currPid] != NULL)
            result = evaluator_evaluate((*process_table[currPid]).code, (*process_table[currPid]).last_PC);

            pthread_mutex_lock(&table_mutex);
            if(process_table[currPid] != NULL)
              (*process_table[currPid]).last_PC = result.PC;
            pthread_mutex_unlock(&table_mutex);

            if(result.reason == reason_blocked) {
              pthread_mutex_lock(&table_mutex);
              if(process_table[currPid] != NULL)
                (*process_table[currPid]).state = blocked;
              pthread_mutex_unlock(&table_mutex);

              pthread_mutex_lock(&event_queue_mutex);
              if(process_table[currPid] != NULL)
                non_blocking_queue_push(eventQueue, currPid);
              pthread_mutex_unlock(&event_queue_mutex);
              pthread_cond_signal(&wait_cond);

            } else if(result.reason == reason_terminated) {
              pthread_mutex_lock(&table_mutex);
              if(process_table[currPid] != NULL)
                (*process_table[currPid]).state = terminated;
              pthread_mutex_unlock(&table_mutex);

              pthread_cond_signal(&term_cond);
              if(non_blocking_queue_length(readyQueue) == 1) {
                pthread_mutex_lock(&table_mutex);
                  pthread_cond_wait(&table_condition, &table_mutex);
                pthread_mutex_unlock(&table_mutex);
              }
              
            } else if(result.reason == reason_timeslice_ended) {
              pthread_mutex_lock(&table_mutex);
              if(process_table[currPid] != NULL)
                if((*process_table[currPid]).state != terminated)
                  (*process_table[currPid]).state = ready;
              pthread_mutex_unlock(&table_mutex);
              pthread_cond_signal(&table_access_cond);

              if(process_table[currPid] != NULL) 
                if((*process_table[currPid]).state != terminated) {
                pthread_mutex_lock(&buffer_mutex);
                  snprintf(buf, 50, "Process %d timeslice ended, re-queuing", currPid);
                  logger_write(buf);
                pthread_mutex_unlock(&buffer_mutex);
                }

              pthread_mutex_lock(&queue_mutex);
              if(process_table[currPid] != NULL)
                non_blocking_queue_push(readyQueue, currPid);
              pthread_mutex_unlock(&queue_mutex);
              //pthread_mutex_unlock(&wait_mutex);

          }
        } 
        /*else if((*process_table[currPid]).state == terminated) {
          //printf("process %d term\n", currPid);
          process_terminate = 1;
          pthread_cond_signal(&term_cond);
          
        }*/
          }
        } 
            
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
  //while(run) {
  for(int i = 0; i < maxProcess; i++) {
    if(process_table[i] == NULL) {
      pthread_mutex_lock(&table_mutex);
      process_table[i] = malloc(sizeof(ProcessControlBlockT));
      pthread_mutex_unlock(&table_mutex);
      
      pid = i;
     // processCount++;

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
    //pthread_mutex_unlock(&table_mutex);
  }

  //pthread_cond_wait(&qu_empty_cond, &qu_mutex);

  //}
  //pthread_mutex_unlock(&qu_mutex);
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
  
  found = 0;
      //pthread_mutex_lock(&table_mutex);
        //pthread_cond_wait(&term_cond, &table_mutex);
      
      if(process_table[pid] != NULL && (*process_table[pid]).state == terminated) {
        pthread_mutex_lock(&table_mutex);
          if(process_table[pid] != NULL && (*process_table[pid]).state == terminated) {
            free(process_table[pid]);
            process_table[pid] = NULL;
          }
        pthread_mutex_unlock(&table_mutex);
        pthread_cond_signal(&table_condition);
        pthread_cond_signal(&qu_empty_cond);
          //pthread_mutex_unlock(&table_mutex);

        pthread_mutex_lock(&buffer_mutex);
          snprintf(buf, 50, "Process id: %d finished", pid);
          logger_write(buf);
        pthread_mutex_unlock(&buffer_mutex);
        
        return;
      }
      /*if(process_table[pid] != NULL && (*process_table[pid]).state == blocked) {
        pthread_mutex_lock(&wait_mutex);
        pthread_cond_wait(&wait_cond, &wait_mutex);
        pthread_mutex_unlock(&wait_mutex);
        printf("waiting FOR ASD\n");
      }*/
    }

  }

void simulator_kill(ProcessIdT pid) {
  char buf[50];
  int kill_found = 1;
      pthread_mutex_lock(&table_mutex);
      if(process_table[pid] != NULL)
          (*process_table[pid]).state = terminated;
      pthread_mutex_unlock(&table_mutex);
      //pthread_cond_signal(&term_cond);
      //process_terminate = 1;

      pthread_mutex_lock(&buffer_mutex);
      snprintf(buf, 50, "Process id: %d killed", pid);
      logger_write(buf);
      pthread_mutex_unlock(&buffer_mutex);

}

void simulator_event() {
  //while(!non_blocking_queue_empty(eventQueue)) {
    ProcessIdT currPid;
    char buf[50];

    if((!non_blocking_queue_empty(eventQueue))){
    pthread_mutex_lock(&event_queue_mutex);
    if((!non_blocking_queue_empty(eventQueue)))
      non_blocking_queue_pop(eventQueue, &currPid);
    pthread_mutex_unlock(&event_queue_mutex);

    pthread_mutex_lock(&table_mutex);
        if(process_table[currPid] != NULL)
          (*process_table[currPid]).state = ready;
    pthread_mutex_unlock(&table_mutex);

    pthread_mutex_lock(&queue_mutex);
      if(process_table[currPid] != NULL)
       non_blocking_queue_push(readyQueue, currPid);
    pthread_mutex_unlock(&queue_mutex);

    pthread_mutex_lock(&buffer_mutex);
    snprintf(buf, 50, "Process id: %d moved to ready queue", currPid);
    logger_write(buf);
    pthread_mutex_unlock(&buffer_mutex);
 }
}
