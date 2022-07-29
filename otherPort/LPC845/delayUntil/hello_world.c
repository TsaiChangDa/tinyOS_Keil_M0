#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"


int a = 0;
int b = 1;
int c = 0;
int d = 0;

void task0(void)  
{  
   while(1) 
   { 
       delayUntilEqualOS(&a, &b);
	     PUTCHAR('a');	
       a = 0;	
       c = 0;
       d = 0;		 
    } 
} 

void task1(void)  
{
   while(1) 
   { 
       PUTCHAR('B');	
		   delayTickOS(10);	
       c++;		 
    } 
} 

void task2(void)  
{
   while(1) 
   { 
       PUTCHAR('C');	
		   delayTickOS(10);	
       d++;	
		   a = (c+d > 9)? 1 : 0;	 
    } 
} 

void (*taskName[])(void)={task0,task1, task2};


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

	  return 0;
}
