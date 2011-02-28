#define USE_SESC

#ifdef USE_SESC
   #include "sescapi.h"
#else
   #include <pthread.h>
#endif

#include <stdio.h>
//#include <stdlib.h>

/*
#define SIM_CHECKPOINT 2
#define SIM_START_RABBIT 0
#define SIM_STOP_RABBIT 1

#define TM_SIMOP(x) asm __volatile__ ("andi $0, $0, %0\n\t"::"i"(x): "memory")
*/

/*
 * Calling TM_SIMOP(SIM_CHECKPOINT) will take a chekcpoint.
 * Calling TM_SIMOP(SIM_STOP_RABBIT) will end rabbit mode.
 * Calling TM_SIMOP(SIM_START_RABBIT) will start rabbit mode.
 * */

volatile int myInt = 1;

void *readInt(void *threadid)
{
   int i=0;
   //int localInt = 0;
	printf("readIntThreadid=%d\n", *((int*)threadid));
   
   for (i=0; i<10000; i++)
   {
      //localInt = myInt;
      fprintf(stderr, "%d", myInt);
   }

   printf("\n");
#ifdef USE_SESC
   sesc_exit(0);
#else
    pthread_exit(NULL);
#endif
}

void *writeInt(void *threadid)
{
   int i=0;
	printf("writeIntThreadid=%d\n", *((int*)threadid));
   
   for (i=0; i<10000; i++)
   {
      myInt = myInt + 1;
   }

   printf("finalWriteInt=%d\n", myInt);
#ifdef USE_SESC
   sesc_exit(0);
#else
    pthread_exit(NULL);
#endif
}


int main(int argc, char** argv)
{
#ifdef USE_SESC
	sesc_init();
#else
    pthread_t readthread;
    pthread_t writethread;
#endif
   int readthreadid = 0;
   int writethreadid = 1;

   printf("Creating read thread %d\n", readthreadid);
#ifdef USE_SESC
   sesc_spawn((void (*)(void*)) *readInt, &readthreadid, 0);
#else
   pthread_create(&readthread, NULL, readInt, &readthreadid);
#endif
 
   printf("Creating write thread %d\n", writethreadid);
#ifdef USE_SESC
   sesc_spawn((void (*)(void*)) *writeInt, &writethreadid, 0);
#else
   pthread_create(&writethread, NULL, writeInt, &writethreadid);
#endif

#ifdef USE_SESC
   sesc_wait(); // wait for threads to finish
	sesc_exit(0);
#else
   pthread_join(readthread, NULL);
   pthread_join(writethread, NULL);
   pthread_exit(NULL);
#endif
}

