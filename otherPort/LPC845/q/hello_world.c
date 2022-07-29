
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"


int     *pint;
char    *pchar;
float   *pfloat;
double  *pdouble;
void**  pvoid;

int     x = 1;
float   y = (float)0xaa;

void task0(void)  
{	
   while(1)
   {
		   PUTCHAR('A');
       pint = getMemoryOS(4, 0);
		   *pint = x++;
		 	 postQOS(0, pint);

		   pchar = getMemoryOS(1, 1);		 
		   *pchar = '0' + x;
		 	 postQOS(0, pchar);
		 
		   pdouble = getMemoryOS(8, 2);				 
		   *pdouble     = 0.00000016;
		   *(pdouble+1) = 0.00000032;
		 	 postQOS(0, pdouble);		 
		 
		   delayTickOS(15);
   }
}

void task1(void)  
{
   while(1)
   {
		   PUTCHAR('B');
       pfloat = getMemoryOS(4, 3);	 
		   *pfloat = y;
		   y += 1.0;
		 	 postQOS(1,pfloat); 
		   delayTickOS(15);
   }
}

void task2(void)  
{	 
	 int           number[]={1, 0, -1};
	 double        w1, w2;
	 
   while(1)
   {
		   PUTCHAR('C');
		 	 pvoid = pendQOS(number, 1, TIMEOUT_INFINITE );
		   if ( getQNumberOS(pvoid) == 1 )
			 {
           pfloat = *pvoid;
				   PUTCHAR('@');
			     PRINTF( "%x ", (int) *pfloat );	
		 	 }

       pvoid = readQOS(0);
		   pint    =  (int*)    *pvoid;
		   pchar   =  (char*)   *(pvoid + 1);
		   pdouble =  (double*) *(pvoid + 2);
				 
		 	 PRINTF( "%x ",  *pint );	
		 	 PUTCHAR( *pchar );	
		 
		 	 w1 = *pdouble * 100000000.0;              // 16 , 0x10 
			 w2 = *(pdouble + 1) * 100000000.0;			   // 32 , 0x20
		 	PRINTF( "%x ", (int)w1  );	
		 	 PRINTF( "%x ", (int)w2  );				 

			 putMemoryOS(0);
			 putMemoryOS(1);
			 putMemoryOS(2);
			 putMemoryOS(3);	

    }
}

void (*taskName[])(void)={task0, task1, task2};



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
