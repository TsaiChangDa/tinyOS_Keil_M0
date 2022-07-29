
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>

 #include "OS.h"

void task0(void)  
{	int n[]={10, -1}; while(1)   {  PUTCHAR('A'); PUTCHAR('0');  pendSemOS(n, TIMEOUT_INFINITE  );  PUTCHAR('a'); delayTimeOS(0,0,0, 400);   }   } 
void task1(void)  
{ int n[]={11, -1}; while(1)   {  PUTCHAR('A'); PUTCHAR('1');	 pendSemOS(n, TIMEOUT_INFINITE  );  PUTCHAR('a'); delayTimeOS(0,0,0, 400);  }   } 
void task2(void)  
{	int n[]={12, -1}; while(1)   {  PUTCHAR('A'); PUTCHAR('2');	 pendSemOS(n, TIMEOUT_INFINITE  );	PUTCHAR('a'); delayTimeOS(0,0,0, 400);  }   } 
void task3(void)  
{	int n[]={13, -1}; while(1)   {  PUTCHAR('A'); PUTCHAR('3');	 pendSemOS(n, TIMEOUT_INFINITE  );  PUTCHAR('a');  delayTimeOS(0,0,0, 400);  }   } 
void task4(void)  
{	int n[]={14, -1}; while(1)   {  PUTCHAR('A'); PUTCHAR('4');	 pendSemOS(n, TIMEOUT_INFINITE  );	PUTCHAR('a'); delayTimeOS(0,0,0, 400);  }   } 

void task5(void)  
{	int n[]={20, -1}; while(1)   {  PUTCHAR('B'); PUTCHAR('5');	 pendSemOS(n, TIMEOUT_INFINITE  );  PUTCHAR('b'); delayTimeOS(0,0,0, 400);  }   } 
void task6(void)  
{	int n[]={21, -1}; while(1)   {  PUTCHAR('B'); PUTCHAR('6');	 pendSemOS(n, TIMEOUT_INFINITE  );	PUTCHAR('b'); delayTimeOS(0,0,0, 400);  }   } 
void task7(void)  
{	int n[]={22, -1}; while(1)   {  PUTCHAR('B'); PUTCHAR('7');	 pendSemOS(n, TIMEOUT_INFINITE  );	PUTCHAR('b'); delayTimeOS(0,0,0, 400);  }   } 
void task8(void)  
{	int n[]={23, -1}; while(1)   {  PUTCHAR('B'); PUTCHAR('8');	 pendSemOS(n, TIMEOUT_INFINITE  );	PUTCHAR('b'); delayTimeOS(0,0,0, 400);  }   } 
void task9(void)  
{	int n[]={24, -1}; while(1)   {  PUTCHAR('B'); PUTCHAR('9');	 pendSemOS(n, TIMEOUT_INFINITE  );	PUTCHAR('b'); delayTimeOS(0,0,0, 400);  }   } 

void task10(void)  
{
   int n[]={0,1,2,3,4, -1}; 
	 int x;
	 
   while(1) 
   { 
      PUTCHAR('P');	
      x = pendSemOS(n, TIMEOUT_INFINITE  );	
		  PUTCHAR('p');	
      postSemOS(x+10); 	
    } 
} 
void task11(void)  
{
   int n[]={0,1,2,3,4, -1}; 
	 int x;
	 
   while(1) 
   { 
      PUTCHAR('Q');	
      x = pendSemOS(n, TIMEOUT_INFINITE  );	
		  PUTCHAR('q');	
      postSemOS(x+20); 			 
    } 
} 

void task12(void)  
{	
	while(1)
  { 
    PUTCHAR('R');	
     postSemOS(0);   delayTickOS(10);
     postSemOS(1);   delayTickOS(10);
     postSemOS(2);   delayTickOS(10);
     postSemOS(3);   delayTickOS(10); 
     postSemOS(4);   delayTickOS(10); 
  } 
} 


void (*taskName[])(void)={task0,task1,task2,task3,task4,task5,task6,task7,task8,task9,task10,task11, task12};

	


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
