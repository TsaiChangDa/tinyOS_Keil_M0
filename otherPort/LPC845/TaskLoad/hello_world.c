
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"


void forLoop(int f)
{
    int i, j;
	  int tick = 10;
	
    for (i=0; i<f; i++)
		{
         for (j=0; j<10000; j++)
		     {
				      tick = tick;
			    }
		 }	
}

int   d = 9;
int   f = 4;

void task0(void)   
{
	   while(1)
     {
		     PUTCHAR('A');
			 	 postQOS(0, 0x0);
         forLoop(f);			 
				 delayTickOS(d);
     }
} 

void task1(void)   
{
	   while(1)
     {
		     PUTCHAR('B');
			 	 postQOS(0, 0x0);
         forLoop(f);			 
				 delayTickOS(d);
     }
}

void task2(void)   
{
	   while(1)
     {
		     PUTCHAR('C');
			 	 postQOS(0, 0x0);
         forLoop(f);			 
				 delayTickOS(d);
     }
}

void task3(void)   
{
	   while(1)
     {
		    PUTCHAR('D');
			 	 postQOS(0, 0x0);
         forLoop(f);			 
				 delayTickOS(d);
     }
}

void task4(void)   
{
	   while(1)
     {
		    PUTCHAR('E');
			 	 postQOS(0, 0x0);
         forLoop(f);				 
				 delayTickOS(d);
     }
}

void task5(void)   
{
	   while(1)
     {
		     PUTCHAR('F');
			 	 postQOS(0, 0x0);
         forLoop(f);			 
				 delayTickOS(d);
     }
}

void task6(void)   
{
	   while(1)
     {
		    PUTCHAR('G');
			 	 postQOS(0, 0x0);
         forLoop(f);			 
				 delayTickOS(d);
     }
}

void task7(void)   
{
	   while(1)
     {
		     PUTCHAR('H');
			 	 postQOS(0, 0x0);
         forLoop(f);		 
				 delayTickOS(d);
     }
}

void task8(void)  
{	
	   int  number[] = {0, -1};
	
	   while(1)
     { 
		      PUTCHAR('I');			 
		      pendQOS(number, 1, TIMEOUT_INFINITE );
			    forLoop(f);		
				  delayTickOS(d);
     }
} 

void task9(void)  
{	
	  int i;
	  int *yy;
	
	   while(1)
     {
			  yy = relativeTaskLoadOS();
			  for(i=0; i<=9; i++)
			  {
					 PUTCHAR('0'+ i);	
				  PRINTF("%x ",  *(yy+i) );
			  }
				PUTCHAR('@');	
				PRINTF("%x ", idleTaskLoadOS() );
				
				 delayTickOS(d/3);	 
     }
}

void (*taskName[])(void)={task0,task1,task2,task3,task4,task5,task6,task7,task8,task9};



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
