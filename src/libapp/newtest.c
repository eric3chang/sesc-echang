#define USE_SESC

#ifdef USE_SESC
   #include "sescapi.h"
#else
   #define MAX_NUM_THREADS 64
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

void *readInt(void *numberOfLoops)
{
   int i=0;
   int localInt = 0;
	//printf("readIntThreadid=%d\n", *((int*)threadid));
   
   for (i=0; i<*((int*)numberOfLoops); i++)
   {
      localInt = myInt;
   }
#ifdef USE_SESC
   sesc_exit(0);
#else
   pthread_exit(NULL);
#endif
}

void *writeInt(void *numberOfLoops)
{
   int i=0;
   for (i=0; i<*((int*)numberOfLoops); i++)
   {
      myInt = myInt + 1;
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
#else
   pthread_t threads[MAX_NUM_THREADS];
#endif
   char *tempString = NULL;
   int t = 0;
   int threadIndexMax = 0;
   int numberOfLoops = 0;
   int numberOfOperations = 0;
   int processorCount = 0;

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
            numberOfOperations = atoi(tempString+2);
         }
      }
   }

   if (!processorCount || !numberOfOperations)
   {
      printf("Usage: readtest -p[number of processors] -n[number of operations]\n");
#ifdef USE_SESC
      sesc_exit(1);
#else
      pthread_exit(NULL);
#endif
   }

   threadIndexMax = processorCount - 1;

   // calculate the number of Loops
   numberOfLoops = numberOfOperations / processorCount;
   printf("numberOfLoops=%d\n", numberOfLoops);

   t = 0;
   while (1)
   {
      t++;
      printf("Creating read thread %d\n", t);
#ifdef USE_SESC
      sesc_spawn((void (*)(void*)) *readInt, &numberOfLoops, 0);
#else
      pthread_create(&threads[t], NULL, readInt, &numberOfLoops);
#endif

      if (t >= threadIndexMax)
      {
         break;
      }
      t++;
      printf("Creating write thread %d\n", t);
#ifdef USE_SESC
      sesc_spawn((void (*)(void*)) *writeInt, &numberOfLoops, 0);
#else
      pthread_create(&threads[t], NULL, writeInt, &numberOfLoops);
#endif
   }

   printf("Creating write thread %d\n", t+1);
   writeInt(&numberOfLoops);

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

