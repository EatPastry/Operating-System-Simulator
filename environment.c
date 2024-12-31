//Student: Joshua Gaynor ID: 20549366
#include "environment.h"
#include "simulator.h"
#include "utilities.h"
#include "evaluator.h"
#include "list.h"
#include <pthread.h>
#include <stdio.h>

pthread_t* env_threads;
int* env_args;
unsigned int public_thread_count, public_iterations, public_batch_size;

void* terminating_routine(void* arg) {
	ProcessIdT pid;
	printf("test %d\n", *((int*)arg));
	for(int i = 0; i < public_iterations; i++) {
		for(int j = 0; j < public_batch_size; j++) {
			pid = simulator_create_process(evaluator_terminates_after(5));
			//usleep(5);
			simulator_wait(pid);
		}
		
	}
	return 0;
}

void environment_start(unsigned int thread_count,
		       unsigned int iterations,
		       unsigned int batch_size) {
	printf("%d %d %d\n", iterations, batch_size, thread_count);
	public_batch_size = batch_size;
	public_iterations = iterations;
	public_thread_count = thread_count;
	env_args = malloc(sizeof(int) * thread_count);
	env_threads = malloc(sizeof(pthread_t) * thread_count);
	for(int i = 0; i < thread_count; i++) {
		env_args[i] = i;
		if(pthread_create(env_threads + i, NULL, terminating_routine, env_args + i)){
			return;
		}
	}
}

void environment_stop() {
	for(int i = 0; i<public_thread_count; i++) {
		pthread_join(env_threads[i], NULL);
	}
	free(env_threads);
	free(env_args);
}
