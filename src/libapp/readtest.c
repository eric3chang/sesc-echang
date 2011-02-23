#define USE_SESC

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_SESC
   #include "sescapi.h"
#else
   #include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>
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

//unsigned char myCharArray [2];
unsigned char myChar = 'a';
unsigned char myChar2 = 'A';
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
void *readChar(int *threadid)
{
   int i=0;
	printf("threadid=%d\n", *threadid);
   for (i=0; i<100; i++)
   {
      if (*threadid)
      {
         myChar = myChar + 1;
         // uppercase letters
         if (myChar<65 || myChar>90)
         {
            myChar = 65;
            printf("\n");
         }
         //myChar = myChar % 93;
         //myChar += 33;
         printf("%c", myChar);
      }
      else
      {
         myChar2 = myChar2 + 1;
         // lowercase letters
         if (myChar2<97 || myChar2>122)
         {
            myChar2 = 97;
            printf("\n");
         }
         //myChar2 = myChar2 % 93;
         //myChar2 += 33;
         printf("%c", myChar2);
      }
   }

   printf("\n");
//   printf("%s\n", myCharArray);
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
#ifdef USE_SESC
      sesc_exit(1);
#endif
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
   for(t=0;t<processorCount;t++)
   {
       printf("Creating thread %d\n", t);
#ifdef USE_SESC
	    sesc_spawn((void (*)(void*)) *readChar, &t, 0);
	    //sesc_spawn((void (*)(void*)) *readChar, &numberOfLoops , 0);
#else
       pthread_create(&threads[t], NULL, readChar, 5);
#endif
//	    sesc_spawn((void (*)(void*)) *readChar, myIntArray, 0);
//       *args[0] = t;
//	    sesc_spawn((void (*)(void*)) *readChar, args, 0);
//	    sesc_spawn((void (*)(void*)) *readChar, sema, 0);
    }
#ifdef USE_SESC
   sesc_wait(); // wait for thread to finish
	sesc_exit(0);
#else
    pthread_exit(NULL);
#endif
//   free(myCharArray);
}

#ifdef __cplusplus
}
#endif

