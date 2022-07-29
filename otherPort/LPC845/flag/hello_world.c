#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"


unsigned int post0 = 0x00000001;  // bit 0   index 0x00
unsigned int post1 = 0x00000100;  // bit 8   index 0x08
unsigned int post2 = 0x00010000;  // bit 16  index 0x10
unsigned int post3 = 0x01000000;  // bit 24  index 0x18


void task0(void)   
{ 
	  while(1)
    {
		    PUTCHAR('A');
			
				postFlagOS(0, post0 , FLAG_SET);
        delayTickOS(20);
    }
} 

void task1(void)   
{
	  while(1)
    {
		    PUTCHAR('B');

				postFlagOS(0, post1 , FLAG_SET);
        delayTickOS(15);
    }
} 

void task2(void)  
{
	  while(1)
    {
		    PUTCHAR('C');

				postFlagOS(0, post2 , FLAG_SET);
        delayTickOS(10);
    }
} 

void task3(void)   
{
	  while(1)
    {
		    PUTCHAR('D');

				postFlagOS(0, post3 , FLAG_SET);
        delayTickOS(5);
    }
} 

void task4(void)   
{
    int number[] = {0 , -1};
		int i;
		char bit;
    unsigned int pendFlag = 0xffffffff;  
		
	  while(1)
    {
		    PUTCHAR('E');
		 
			  pendFlagOS(number, pendFlag, FLAG_MATCH_ANY, TIMEOUT_INFINITE );
			
			  for(i=0; i<32; i++)
			  {
		        bit = checkPublicFlagBitOS(0, i);
					  if( bit )
						{
					      PRINTF("ox %x ", i );  // bit index
						}
				}
				PRINTF("ox %x ", queryPublicFlagOS(0) ); // public flag value
				
				postFlagOS(0, 0xffffffff , FLAG_CLEAR);
    }
} 

void (*taskName[])(void)={task0,task1,task2,task3,task4};



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
