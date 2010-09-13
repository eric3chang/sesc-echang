#ifdef __cplusplus
extern "C" {
#endif

//#include <pthread.h>
#include "sescapi.h"
#include <stdio.h>
#include <stdlib.h>
//#define NUM_THREADS        5

#define SIM_CHECKPOINT 2
#define SIM_START_RABBIT 0
#define SIM_STOP_RABBIT 1

#define TM_SIMOP(x) asm __volatile__ ("andi $0, $0, %0\n\t"::"i"(x): "memory")

/*
 * Calling TM_SIMOP(SIM_CHECKPOINT) will take a chekcpoint.
 * Calling TM_SIMOP(SIM_STOP_RABBIT) will end rabbit mode.
 * Calling TM_SIMOP(SIM_START_RABBIT) will start rabbit mode.
 * */

//unsigned char myCharArray [2];
unsigned char myChar = 'a';
//int counter = 0;
unsigned char *myCharArray;
//int counter = -1;
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
// ends rabbit mode

/* cannot use any for loops in child threads for some reason
 * even trivial for loops cause the program to fail
 */
void *readChar(int *numberOfLoops)
{
   int i=0;
	printf("number of loops=%d\n", *numberOfLoops);
   printf("%s\n", myCharArray);
//    pthread_exit(NULL);
	sesc_exit(0);
}

int main(int argc, char** argv)
{
	sesc_init();
//    pthread_t threads[NUM_THREADS];
	int t = 0;
   int processorCount = 0;
   int numberOfLoops = 1;
   char *tempString = NULL;
   unsigned char tempChar = 0;

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
      sesc_exit(1);
   }

   //TM_SIMOP(SIM_STOP_RABBIT);
   myCharArray = (unsigned char*)malloc(sizeof(unsigned char) * numberOfLoops);
   for (t=0; t<numberOfLoops-1; t++)
   {
      // do modifications to get readable characters
      tempChar = t % 93;
      tempChar += 33;
      myCharArray[t] = tempChar;
   }
   // insert string end character
   myCharArray[numberOfLoops-1] = 0;
   /* 
   sema = (ssema_t*)malloc(sizeof(ssema_t));

   sesc_sema_init(sema, 1);
*/
   for(t=0;t<processorCount;t++){
       printf("Creating thread %d\n", t);
//       pthread_create(&threads[t], NULL, print_hello_world, (void *)t);
	    sesc_spawn((void (*)(void*)) *readChar, &numberOfLoops , 0);
//	    sesc_spawn((void (*)(void*)) *readChar, myIntArray, 0);
//       *args[0] = t;
//	    sesc_spawn((void (*)(void*)) *readChar, args, 0);
//	    sesc_spawn((void (*)(void*)) *readChar, sema, 0);
    }
   // wait for thread to finish
   sesc_wait();
//    pthread_exit(NULL);
   free(myCharArray);
	sesc_exit(0);
}

#ifdef __cplusplus
}
#endif

