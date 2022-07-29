
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"


void accessArray(int *array, int length)
{
   int i;
	
	 for (i=0; i<length; i++)
   {
		  *(array + i) = 7;
		  *(array + i) =  *(array + i);		 
	 }
}

void task0(void)  
{	 
	 int arr[64];

   accessArray(arr, 64);
	
   while(1)
   { 
		   PUTCHAR('A');

		   delayTickOS(16);				 
    }
}

void task1(void)  
{
	 int arr[32];

   accessArray(arr, 32);
	
   while(1)
   {	
		   PUTCHAR('B');

		   delayTickOS(16);			 
    }
}

void task2(void)  
{	 
	 int arr[16];
	
	 accessArray(arr, 16);
	
   while(1)
   { 
		   PUTCHAR('C');

		   delayTickOS(16);		 
    }
}

void task3(void)  
{	 
	 int arr[8];

   accessArray(arr, 8);
	
   while(1)
   { 
		   PUTCHAR('D');

		   delayTickOS(16);		 
    }
}

void task4(void)  
{	 
	 int  i;
   int  *pint;
	
   while(1)
   { 	
		   PUTCHAR('E');
			
      PRINTF("0x%x ", minimumStackOS() );
		 
		   pint = packSizeOS();
		 
		   for(i=0; i<5; i++)
			 {
				   PRINTF("0x%x ", *(pint + i) );		 // task padding
			 }
			 
			 delayTickOS(16);				 
    }
}


void (*taskName[])(void)={task0, task1,task2, task3,task4};



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
