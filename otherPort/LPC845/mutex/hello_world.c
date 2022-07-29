
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"

void task0(void)  
{	
   int number[] = {0, -1};
   int sem[] = {0, -1};
	 
   while(1)
   {
		 		  PUTCHAR('A');
	        pendSemOS(sem, TIMEOUT_INFINITE);	
	        pendMutexOS(number, TIMEOUT_INFINITE);		

		      PUTCHAR('a');
	        postMutexOS();		 
	        postSemOS(1);	
   }
}


void task1(void)  
{	
   int number[] = {0, -1};
   int sem[] = {1, -1};
	 
   while(1)
   {
		      PUTCHAR('B');
	        pendSemOS(sem, TIMEOUT_INFINITE);	
	        pendMutexOS(number, TIMEOUT_INFINITE);		

		      PUTCHAR('b');
	        postMutexOS();		 
	        postSemOS(2);	
   }
}

void task2(void)  
{	
   int number[] = {0, -1};
   int sem[] = {2, -1};
	 
   while(1)
   {
		      PUTCHAR('C');
	        pendSemOS(sem, TIMEOUT_INFINITE);	
	        pendMutexOS(number, TIMEOUT_INFINITE);		

		      PUTCHAR('c');
	        postMutexOS();		 

   }
}


void task3(void)  
{	
   int number[] = {0, -1};
	 
   while(1)
   {
		      PUTCHAR('D');
	        pendMutexOS(number, TIMEOUT_INFINITE);	
	
		      PUTCHAR('d');
	        postMutexOS();	
	        postSemOS(0);			 
		      delayTickOS(6);	

   }
}

void (*taskName[])(void)={task0, task1, task2, task3};




int main(void)
{
	   char errorCode;
	   int startTaskIndex;
	   int arraySize;

    /* Init board hardware. */
    /* Select the main clock as source clock of USART0 (debug console) */
    CLOCK_Select(BOARD_DEBUG_USART_CLK_ATTACH);
    BOARD_InitBootPins();
    BOARD_BootClockFRO30M();
    BOARD_InitDebugConsole();
    /* Turn on LED RED */
    LED_RED_INIT(LOGIC_LED_ON);

	  PUTCHAR('A');
    PRINTF("BCD \r\n");
	
    arraySize = sizeof(taskName) / sizeof(taskName[0]);
    startTaskIndex = 0;
    errorCode = startOS(taskName, arraySize, startTaskIndex, clockOS);  // create idleTaskOS()


   // while (1)
   // {
    //    ch = GETCHAR();
    //    PUTCHAR(ch+3);
   // }
	  return 0;
}
