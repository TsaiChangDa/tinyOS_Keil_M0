
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"


int    *pint;
char   *pchar;
float  *pfloat;
double *pdouble;
int     dangerBytes;

void task0(void)  
{
	 int *p0;
	
   while(1)
   {
		   PUTCHAR('A');
			 p0 = getMemoryOS(100, 0);	
       if(p0 == 0x0)
			 {
          PUTCHAR('@');     // error
			 }				 
		 
		   delayTickOS(12);
   }
}

void task1(void)  
{
	 char *p1;
	
   while(1)
   {
		   PUTCHAR('B');
			 p1 = getMemoryOS(100, 1);	
       if(p1 == 0x0)
			 {
          PUTCHAR('@');     // error
			 }				 
       putMemoryOS(1);
		   delayTickOS(10);
   }
}


void task2(void)  
{	
   while(1)
   {
		  PUTCHAR('C');
	    pfloat = getMemoryOS(100, 2);
		  pint = margeAddressOS(pfloat);
		  pchar = dangerAddressOS(pfloat, &dangerBytes);
		  if( dangerBytes > 0 )     
			{
				  pdouble = (double*)pfloat + 24;
		 
	    	 *pfloat = 0.7;
	    	 *pfloat *= 	10.0;  // 7
				
				 *pdouble = 0.00000008;
	    	 *pdouble *= 	100000000.0;  // 8
				
	    	 *pint = 8;
	    	 *pint += 1;		// 9 
		 
	    	 *pchar = 'X';         
	    	 *(pchar + 150) = 'Y'; 
		 
 	    	  PUTCHAR('0'+ (char)*pfloat );		// 7
 	    	  PUTCHAR('0'+ (char)*pdouble );		// 8				
	       	PUTCHAR('0'+ (char)*pint );		  // 9 
	       	PUTCHAR( *pchar );			          // X
	    		PUTCHAR( *(pchar + 150) );       // Y
					
          PUTCHAR('#'); 				
	       	PRINTF( "%x ",dangerBytes );	
			}			
      else
			{
          PUTCHAR('@');     // error
			}				
		   delayTickOS(9);
   }
}

void task3(void)  
{
	 float *p3;
	
   while(1)
   {
		   PUTCHAR('D');
			 p3 = getMemoryOS(600, 3);	
       if(p3 == 0x0)
			 {
          PUTCHAR('@');     // error
			 }				 
		   delayTickOS(6);
   }
}

void task4(void)  
{	 
   while(1)
   {
		   PUTCHAR('E');
       putMemoryOS(0);   // task0
			 putMemoryOS(2);   // task2
       putMemoryOS(3);   // task3			 
			 
			 delayTickOS(4);		 

    }//while
}

void (*taskName[])(void)={task0, task1, task2, task3, task4};


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
