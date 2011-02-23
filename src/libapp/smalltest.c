#define USE_SESC

#ifdef USE_SESC
   #include "sescapi.h"
#else
   #include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#define SIM_CHECKPOINT 2
#define SIM_START_RABBIT 0
#define SIM_STOP_RABBIT 1

#define TM_SIMOP(x) asm __volatile__ ("andi $0, $0, %0\n\t"::"i"(x): "memory")

/*
 * Calling TM_SIMOP(SIM_CHECKPOINT) will take a chekcpoint.
 * Calling TM_SIMOP(SIM_STOP_RABBIT) will end rabbit mode.
 * Calling TM_SIMOP(SIM_START_RABBIT) will start rabbit mode.
 * */

int finalOutput[65535];
int *finalOutputPtr = finalOutput;
int myChar = 1;

void *readChar(int *threadid)
{
   int i=0;
   int threadidCopy = *threadid;
   myChar = myChar + 1;
	printf("threadid=%d\n", threadidCopy);
   for (i=0; i<10000; i++)
   {
      //printf("%d", threadidCopy);
      *finalOutputPtr = threadidCopy;
      finalOutputPtr++;
   }

   printf("\n");
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
    pthread_t threads[NUM_THREADS];
#endif
   int* index = 0;
	int t = 0;
   int processorCount = 0;
   int numberOfLoops = 0;
   char *tempString = NULL;

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
            numberOfLoops = atoi(tempString+2);
         }
      }
   }

   if (!processorCount || !numberOfLoops)
   {
      printf("Usage: readtest -p[number of processors] -n[number of loops]\n");
#ifdef USE_SESC
      sesc_exit(1);
#endif
   }

   for(t=0;t<processorCount;t++)
   {
       printf("Creating thread %d\n", t);
#ifdef USE_SESC
	    sesc_spawn((void (*)(void*)) *readChar, &t, 0);
#else
       pthread_create(&threads[t], NULL, readChar, &t);
#endif
    }
#ifdef USE_SESC
   sesc_wait(); // wait for thread to finish
#endif

   /*
   for (index=finalOutput; index<finalOutputPtr; index++)
   {
      printf("%d", *index);
   }
   */
   printf("\n");

#ifdef USE_SESC
	sesc_exit(0);
#else
   pthread_exit(NULL);
#endif
}

