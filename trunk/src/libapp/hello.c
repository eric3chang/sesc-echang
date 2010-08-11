//#include <pthread.h>
#include "sescapi.h"
#include <stdio.h>
#define NUM_THREADS        5

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
