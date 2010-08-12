//#include <pthread.h>
#include "sescapi.h"
#include <stdio.h>
#define NUM_THREADS        5

#define SIM_CHECKPOINT 2
#define SIM_START_RABBIT 0
#define SIM_STOP_RABBIT 1

#define TM_SIMOP(x) asm __volatile__ ("andi $0, $0, %0\n\t"::"i"(x): "memory")

/*
 * Calling TM_SIMOP(SIM_CHECKPOINT) will take a chekcpoint.
 * Calling TM_SIMOP(SIM_STOP_RABBIT) will end rabbit mode.
 * Calling TM_SIMOP(SIM_START_RABBIT) will start rabbit mode.
 * */

void *print_hello_world(void *threadid)
{
	printf("\n%d: Hello World!\n", threadid);
//    pthread_exit(NULL);
	sesc_exit(0);
}

int main()
{
//    pthread_t threads[NUM_THREADS];
	int t;
	sesc_init();
    for(t=0;t<NUM_THREADS;t++){
       printf("Creating thread %d\n", t);
//       pthread_create(&threads[t], NULL, print_hello_world, (void *)t);
	   sesc_spawn((void*) *print_hello_world,(void *) t,0);
    }
//    pthread_exit(NULL);
	sesc_exit(0);
}

