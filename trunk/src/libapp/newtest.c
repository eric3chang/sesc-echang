#ifdef USE_SESC
   #include "sescapi.h"
#else
   #define MAX_NUM_THREADS 32
   #include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>

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
#ifdef USE_SESC
slock_t myLock;
#else
pthread_mutex_t myLock;
#endif

void *readInt(void *numberOfLoops)
{
   int i=0;
   int localInt = 0;
	//printf("readIntThreadid=%d\n", *((int*)threadid));
   
   for (i=0; i<*((int*)numberOfLoops); i++)
   {
#ifdef USE_SESC
      sesc_lock(&myLock);
#else
      pthread_mutex_lock(&myLock);
#endif
      localInt = myInt;
#ifdef USE_SESC
      sesc_unlock(&myLock);
#else
      pthread_mutex_unlock(&myLock);
#endif
   }
#ifdef USE_SESC
   sesc_exit(0);
#else
   pthread_exit(NULL);
#endif
}

void *incInt(void *numberOfLoops)
{
   int i=0;
   for (i=0; i<*((int*)numberOfLoops); i++)
   {
#ifdef USE_SESC
      sesc_lock(&myLock);
#else
      pthread_mutex_lock(&myLock);
#endif
      myInt = myInt + 1;
#ifdef USE_SESC
      sesc_unlock(&myLock);
#else
      pthread_mutex_unlock(&myLock);
#endif
   }
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
   sesc_lock_init(&myLock);
#else
   pthread_t threads[MAX_NUM_THREADS];
   int error = pthread_mutex_init(&myLock,0);
   if (error)
   {
      printf("Error initializing lock\n");
      return(1);
   }
#endif
   char *tempString = NULL;
   int t = 0;
   int threadIndexMax = 0;
   int numberOfLoops = 0;
   int numberOfReads = 0;
   int processorCount = 0;
   int localInt = 0;

   // process arguments
   for (t=0; t<argc; t++)
   {
      tempString = argv[t];
      if (tempString[0]=='-')
      {
         if (tempString[1]=='p')
         {
            processorCount = atoi(tempString+2);
         }
         else if (tempString[1]=='n')
         {
            numberOfReads = atoi(tempString+2);
         }
      }
   }

   if (!processorCount || !numberOfReads)
   {
      printf("Usage: readtest -p[number of processors] -n[number of reads]\n");
#ifdef USE_SESC
      sesc_exit(1);
#else
      pthread_exit(NULL);
#endif
   }

   threadIndexMax = processorCount - 2;
   threadIndexMax = threadIndexMax / 2;

   /* 
   if (!threadIndexMax)
   {
#ifdef USE_SESC
      sesc_exit(0);
#else
      pthread_exit(NULL);
#endif
   }
 */

   // calculate the number of Loops
   //numberOfLoops = numberOfReads / (processorCount-2);
   numberOfLoops = numberOfReads / processorCount;
   printf("numberOfLoops=%d\n", numberOfLoops);

   t = 0;
   while (1)
   {
      printf("Creating write thread %d\n", t);
#ifdef USE_SESC
      sesc_spawn((void (*)(void*)) *incInt, &numberOfLoops, SESC_FLAG_NOMIGRATE|SESC_FLAG_MAP|t+1);
#else
      pthread_create(&threads[t], NULL, incInt, &numberOfLoops);
#endif
      t++;
      if (t >= processorCount-1)
      {
         break;
      }
      printf("Creating read thread %d\n", t);
#ifdef USE_SESC
      sesc_spawn((void (*)(void*)) *readInt, &numberOfLoops, SESC_FLAG_NOMIGRATE|SESC_FLAG_MAP|t+1);
#else
      pthread_create(&threads[t], NULL, readInt, &numberOfLoops);
#endif
      t++;
   }

   printf("Creating read thread %d\n", t);

   // begin readInt section
   for (t=0; t<numberOfLoops; t++)
   {
#ifdef USE_SESC
      sesc_lock(&myLock);
#else
      pthread_mutex_lock(&myLock);
#endif
      localInt = myInt;
#ifdef USE_SESC
      sesc_unlock(&myLock);
#else
      pthread_mutex_unlock(&myLock);
#endif
   }
   // end readInt section

#ifdef USE_SESC
   sesc_wait(); // wait for threads to finish
	sesc_exit(0);
#else
   for (t=0; t<processorCount-2; t++)
   {
      pthread_join(threads[t], NULL);
      pthread_exit(NULL);
   }
#endif
}

