#include <pthread.h>
//#include "sescapi.h"
#include <stdio.h>
#include <stdlib.h>
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

// put this character on the heap instead of on the stack.
// see if the results will be different
char *myChar;
//char myString0[8] = "0000000";
/* 
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

void *readChar()
{
	printf("myChar=%c\n", *myChar);
   *myChar = *myChar + 1;
//   *myChar = *myChar + 1;
    pthread_exit(NULL);
//	sesc_exit(0);
}

int main()
{
    pthread_t threads[NUM_THREADS];
	int t;
   myChar = malloc(sizeof(char) * 1);
   *myChar = 'a';
//	sesc_init();
    for(t=0;t<NUM_THREADS;t++){
       printf("Creating thread %d\n", t);
       pthread_create(&threads[t], NULL, readChar, (void *)t);
//	   sesc_spawn((void*) *readChar, 0, 0);
    }
    pthread_exit(NULL);
//	sesc_exit(0);
}

