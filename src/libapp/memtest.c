//#include <pthread.h>
#include "sescapi.h"
#include <stdio.h>
#define NUM_THREADS        2

#define SIM_CHECKPOINT 2
#define SIM_START_RABBIT 0
#define SIM_STOP_RABBIT 1

#define TM_SIMOP(x) asm __volatile__ ("andi $0, $0, %0\n\t"::"i"(x): "memory")

/*
 * Calling TM_SIMOP(SIM_CHECKPOINT) will take a chekcpoint.
 * Calling TM_SIMOP(SIM_STOP_RABBIT) will end rabbit mode.
 * Calling TM_SIMOP(SIM_START_RABBIT) will start rabbit mode.
 * */

// put this character on the heap instead of on the stack.
// see if the results will be different
//unsigned char myCharArray [2];
//unsigned char myChar = 0;
//int counter = 0;
#define SIZE 2
char *myCharArray;
int counter = -1;
/*
char myString0[8] = "0000000";
char myString1[8] = "1111111";
char myString2[8] = "2222222";
char myString3[8] = "3333333";
char myString4[8] = "4444444";
char myString5[8] = "5555555";
char myString6[8] = "6666666";
char myString7[8] = "7777777";
char myString8[8] = "8888888";
char myString9[8] = "9999999";
*/

void *readChar(ssema_t *sema)
{
   int anotherCounter;
   sesc_psema(sema);
//	printf("myCharArray[%d]=%d\n", threadid, myCharArray[(int)threadid]);
//	printf("myCharArray[%d]=%d\n", counter, myCharArray[(int)counter]);
//    pthread_exit(NULL);
//   counter = counter + 1;
//   printf("counter=%d\n", counter);

//   anotherCounter = counter;
   sesc_vsema(sema);
	sesc_exit(0);
}

int main()
{
//    pthread_t threads[NUM_THREADS];
   char myChar;
	int t,i;
   ssema_t *sema;

   // initialize array
   myCharArray = malloc(sizeof(char) * SIZE);
   for (i=0; i<SIZE; i++)
   {
     myCharArray[i] = i % 128;
   }
   sema = malloc(sizeof(ssema_t));

   sesc_sema_init(sema, 1);

	sesc_init();
   for(t=0;t<NUM_THREADS;t++){
       printf("Creating thread %d\n", t);
//       pthread_create(&threads[t], NULL, print_hello_world, (void *)t);
	    sesc_spawn((void*) *readChar, sema, 0);
    }
//   for (i=0; i<1024*2; i++);
//    pthread_exit(NULL);
   // wait for thread to finish
   sesc_wait();

   free(myCharArray);

	sesc_exit(0);
}

