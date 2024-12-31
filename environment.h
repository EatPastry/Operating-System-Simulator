#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

void* terminating_routine(void* arg);
void environment_start(unsigned int thread_count,
		       unsigned int iterations,
		       unsigned int batch_size);
void environment_stop();

#endif
