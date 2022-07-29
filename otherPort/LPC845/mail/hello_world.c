
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"


char   m0[] = {'4', '5', '6'};
int    m1[] = {7, 8, 9};
char  *pchar;
int   *pint;
void  *pvoid;

void task0(void)   
{
	  while(1)
    {
		     PUTCHAR('A');
	       postMailOS( 0, m0 );	
		 		 delayTickOS(17);	
    }
} 


void task1(void)   
{
	  while(1)
    {
		     PUTCHAR('B');
	       postMailOS( 1, m1 );	
		 		 delayTickOS(13);
    }
} 


void task2(void)   
{
   int  number[] = { 0, 1, -1};
	 int  readynumber;	 
	 int   i;
	 
	 while(1)
   {
			  pvoid = pendMailOS(number, &readynumber, 0, TIMEOUT_INFINITE);
		    PUTCHAR('c');
		 		PUTCHAR('0' + readynumber);

				if( readynumber == 0 )
				{
				   pchar = pvoid;
				   for(i=0; i<3; i++)
		       {
                 PUTCHAR(*(pchar + i) );
					 }
				}
				if( readynumber == 1 )
				{
				   pint = pvoid;
				   for(i=0; i<3; i++)
		       {
                 PUTCHAR('0' + *(pint + i) );
					 }						
        }

		//------------------------------------------------------
			 
				 pchar = readMailOS(0, 1);
				 PUTCHAR('p');
				 if( pchar != 0x0 )
				 {
		         for(i=0; i<3; i++)
		         {
                 PUTCHAR(*(pchar + i) );
             }
			   }
				 else
				 {
					   PUTCHAR( '#' );    // no mail
				 }
		//------------------------------------------------------
				 				 
				 pint = readMailOS(1, 1);
				 PUTCHAR('q'); 
				 if( pint != 0x0 )
				 {
		         for(i=0; i<3; i++)
		         {
                 PUTCHAR('0' + *(pint + i) );
             }	
			   }				 
				 else
				 {
					   PUTCHAR( '#' );   // no mail
				 }
    } // while
} 

void (*taskName[])(void)={task0,task1,task2};


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
