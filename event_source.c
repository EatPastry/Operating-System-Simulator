#include "event_source.h"
#include "utilities.h"
#include "simulator.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <stdio.h>

pthread_t* event_threads;
int i = 0;
int run_event = 1;
void* caller(void* arg) {
    while(run_event) {
        simulator_event();
        usleep(*((useconds_t*)arg));
    }
}
void event_source_start(useconds_t interval) {
    useconds_t args[2];
    args[1] = interval;
    event_threads = malloc(sizeof(pthread_t));
    if(pthread_create(event_threads, NULL, caller, args + 1)) {
        printf("Creating thread failed\n");
    }
}

void event_source_stop() {
    run_event = 0;
    for(int j = 0; j < i+1; j++) {
        pthread_join(event_threads[j], NULL);
    }
    free(event_threads);
}

