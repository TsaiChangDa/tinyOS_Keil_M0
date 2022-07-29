
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"
extern unsigned int* TaskSpPointerOS[5];
 

void task0(void)  
{
   int n[]={0, -1};

	 PRINTF( "%x ", (unsigned int) autoPackItemsOS() ); 
	 
   while(1) 
   {  
		  PUTCHAR('A');
      pendSemOS(n, TIMEOUT_INFINITE  );
      PRINTF( "%x ",  (unsigned int) __get_PSP()); 
 		  PRINTF( "%x ", (unsigned int) *TaskSpPointerOS[0]);	
		  PRINTF( "%x ", (unsigned int) TaskSpPointerOS[0]); 
		  postSemOS(1); 
   }  
} 

void task1(void)  
{ 
   int n[]={1, -1};
	 
   while(1)
   { 
		  PUTCHAR('B');
      pendSemOS(n, TIMEOUT_INFINITE  ); 
      PRINTF( "%x ",  (unsigned int) __get_PSP()); 
 		   PRINTF( "%x ", (unsigned int) *TaskSpPointerOS[1]);	
		   PRINTF( "%x ", (unsigned int) TaskSpPointerOS[1]); 
      postSemOS(2); 
   }  
} 

void task2(void)  
{
   int n[]={2, -1};

   while(1)
   { 
		  PUTCHAR('C');
      pendSemOS(n, TIMEOUT_INFINITE  );
       PRINTF( "%x ",  (unsigned int) __get_PSP()); 
 		  PRINTF( "%x ", (unsigned int) *TaskSpPointerOS[2]);	
		  PRINTF( "%x ", (unsigned int) TaskSpPointerOS[2]); 
      postSemOS(3); 
   }  
} 

void task3(void)  
{	
	  PRINTF( "%x ", (unsigned int) TaskSpPointerOS[4]);	
	
	 while(1)
   { 
     PUTCHAR('D');	
       PRINTF( "%x ",  (unsigned int) __get_PSP()); 
 		  PRINTF( "%x ", (unsigned int) *TaskSpPointerOS[3]);	
		  PRINTF( "%x ", (unsigned int) TaskSpPointerOS[3]); 
		  PUTCHAR('D');	
      postSemOS(0); 
		  delayTimeOS(0,0,1,0);
   } 
} 


void (*taskName[])(void)={task0,task1,task2,task3};

	

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
