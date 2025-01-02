//Student: Joshua Gaynor ID: 20549366
#include "environment.h"
#include "simulator.h"
#include "utilities.h"
#include "evaluator.h"
#include "list.h"
#include <pthread.h>
#include <stdio.h>

pthread_t* env_threads;
pthread_t* inf_threads;
pthread_t* block_threads;
int* inf_args;
int* env_args;
int* block_args;
unsigned int public_thread_count, public_iterations, public_batch_size;

void* terminating_routine(void* arg) {
	ProcessIdT cpid;
	for(int i = 0; i < public_iterations; i++) {
		for(int j = 0; j < public_batch_size; j++) {
			cpid = simulator_create_process(evaluator_terminates_after(5));
			//usleep(5);
			simulator_wait(cpid);
		}
	}
	printf("COMPLETE term\n");
	return 0;
}

void* blocking_routine(void* arg) {
	ProcessIdT cpid;
	for(int i = 0; i < 0; i++) {
		//for(int j = 0; j < public_batch_size; j++) {
			cpid = simulator_create_process(evaluator_blocking_terminates_after(5));
			simulator_wait(cpid);
		//}
	}
	printf("COMPLETE blocking\n");
}

void* infinite_routine(void* arg) {
	ProcessIdT cpid;
	for(int i = 0; i < public_iterations; i++) {
		for(int j = 0; j < public_batch_size; j++) {
			cpid = simulator_create_process(evaluator_infinite_loop);
			simulator_kill(cpid);
			simulator_wait(cpid);
		}
	}
	printf("COMPLETE infinite asdasdasdasdasdasdasdasd\n");
}

void environment_start(unsigned int thread_count,
		       unsigned int iterations,
		       unsigned int batch_size) {
	//printf("%d %d %d\n", iterations, batch_size, thread_count);
	public_batch_size = batch_size;
	public_iterations = iterations;
	public_thread_count = thread_count;
	block_args = malloc(sizeof(int) * thread_count);
	block_threads = malloc(sizeof(pthread_t) * thread_count);
	env_args = malloc(sizeof(int) * thread_count);
	env_threads = malloc(sizeof(pthread_t) * thread_count);
	inf_args = malloc(sizeof(int) * thread_count);
	inf_threads = malloc(sizeof(pthread_t) * thread_count);
	for(int i = 0; i < thread_count; i++) {
		env_args[i] = i;
		if(pthread_create(env_threads + i, NULL, terminating_routine, env_args + i)){
			return;
		}
		inf_args[i] = i;
		if(pthread_create(inf_threads + i, NULL, infinite_routine, inf_args + i)){
			return;
		}
		block_args[i] = i;
		if(pthread_create(block_threads + i, NULL, blocking_routine, block_args + i)){
			return;
		}
		
	}
}

void environment_stop() {
	for(int i = 0; i<public_thread_count; i++) {
		pthread_join(env_threads[i], NULL);
		pthread_join(inf_threads[i], NULL);
		pthread_join(block_threads[i], NULL);
	}
	free(block_threads);
	free(block_args);
	free(env_threads);
	free(env_args);
	free(inf_threads);
	free(inf_args);
}
