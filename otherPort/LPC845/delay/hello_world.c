
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"

 void task0(void)  
{  
   while(1) 
   { 
       //sendByte('A');	
	  PRINTF("hello Morris0.\r\n");
	   delayTickOS(20);
    } 
} 
void task1(void)  
{
   while(1) 
   { 
       //sendByte('B');	
   PRINTF("0x%x \r\n", 64);
		   delayTickOS(10);		
    } 
} 



void (*taskName[])(void)={task0,task1};


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
