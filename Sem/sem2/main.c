
#include "OS.h"

#define  RDR    (1<<0)
#define  THRE   (1<<5)
#define  TEMT   (1<<6)

void  initializeUART(int baudrate)
{
   int Fdiv;
   int regVal;

   NVIC_DisableIRQ(UART_IRQn);
   LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16);	// IOCON clock enable 	
   LPC_IOCON->PIO1_6 |= 0x01;    // RX   PIN-15
   LPC_IOCON->PIO1_7 |= 0x01;    // TX   PIN-16
   LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);	// UART clock enable   
   LPC_SYSCON->UARTCLKDIV = 0x1;    
   LPC_UART->LCR = 0x83;           
   regVal = LPC_SYSCON->UARTCLKDIV;								
   Fdiv = (((SystemCoreClock*LPC_SYSCON->SYSAHBCLKDIV)/regVal)/16)/baudrate ;
   LPC_UART->DLM = Fdiv / 256;							
   LPC_UART->DLL = Fdiv % 256;
   LPC_UART->LCR = 0x03;		
   LPC_UART->FCR = 0x07;		
   regVal = LPC_UART->LSR; 
   while (( LPC_UART->LSR & (THRE|TEMT)) != (THRE|TEMT) );
   while ( LPC_UART->LSR & RDR )
   {
	    regVal = LPC_UART->RBR;	
   }
   NVIC_EnableIRQ(UART_IRQn);
   LPC_UART->IER= (1<<0);     
} //  initUART(int baudrate)


void sendByte(char data)
{
  	while ( !(LPC_UART->LSR & THRE) ); // wait until Tx FIFO is empty
  	LPC_UART->THR = data;
	
  	while ( !(LPC_UART->LSR & THRE) ); // wait until Tx FIFO is empty
  	LPC_UART->THR = ' ';	
}



void transfer(unsigned char  p )
{
	  unsigned char  x;
	
	  if (p < 10) // 0,1 .....9
	  {
		  x = p + 48 ;
	  }
	  else  // 10, 11,....,15
	  {
        x = p + 87;  // a,b,¡K¡K,f
	  }	
    sendByte(x);
}

void print32bits(unsigned  int  y)
{
    unsigned char  p;

	  sendByte( '0' );
	  sendByte( 'x' );	
	  p = (y & 0xF0000000) >>28;
    transfer(p);
	  p = (y & 0x0F000000) >>24;
    transfer(p);
	  p = (y & 0x00F00000) >>20;
    transfer(p);
	  p = (y & 0x000F0000) >>16;
    transfer(p);
    p = (y & 0x0000F000) >>12;
    transfer(p);
	  p = (y & 0x00000F00) >>8;
    transfer(p);
	  p = (y & 0x000000F0) >>4;
    transfer(p);
	  p = (y & 0x0000000F) >>0;
    transfer(p);
		sendByte(' ');
	}

void task0(void)  
{	int n[]={10, -1}; while(1)   {  sendByte('A'); sendByte('0');  pendSemOS(n, INFINITEOS);  sendByte('a'); delayTimeOS(0,0,0, 400);   }   } 
void task1(void)  
{ int n[]={11, -1}; while(1)   {  sendByte('A'); sendByte('1');	 pendSemOS(n, INFINITEOS);  sendByte('a'); delayTimeOS(0,0,0, 400);  }   } 
void task2(void)  
{	int n[]={12, -1}; while(1)   {  sendByte('A'); sendByte('2');	 pendSemOS(n, INFINITEOS);	sendByte('a'); delayTimeOS(0,0,0, 400);  }   } 
void task3(void)  
{	int n[]={13, -1}; while(1)   {  sendByte('A'); sendByte('3');	 pendSemOS(n, INFINITEOS);  sendByte('a');  delayTimeOS(0,0,0, 400);  }   } 
void task4(void)  
{	int n[]={14, -1}; while(1)   {  sendByte('A'); sendByte('4');	 pendSemOS(n, INFINITEOS);	sendByte('a'); delayTimeOS(0,0,0, 400);  }   } 

void task5(void)  
{	int n[]={20, -1}; while(1)   {  sendByte('B'); sendByte('5');	 pendSemOS(n, INFINITEOS);  sendByte('b'); delayTimeOS(0,0,0, 400);  }   } 
void task6(void)  
{	int n[]={21, -1}; while(1)   {  sendByte('B'); sendByte('6');	 pendSemOS(n, INFINITEOS);	sendByte('b'); delayTimeOS(0,0,0, 400);  }   } 
void task7(void)  
{	int n[]={22, -1}; while(1)   {  sendByte('B'); sendByte('7');	 pendSemOS(n, INFINITEOS);	sendByte('b'); delayTimeOS(0,0,0, 400);  }   } 
void task8(void)  
{	int n[]={23, -1}; while(1)   {  sendByte('B'); sendByte('8');	 pendSemOS(n, INFINITEOS);	sendByte('b'); delayTimeOS(0,0,0, 400);  }   } 
void task9(void)  
{	int n[]={24, -1}; while(1)   {  sendByte('B'); sendByte('9');	 pendSemOS(n, INFINITEOS);	sendByte('b'); delayTimeOS(0,0,0, 400);  }   } 

void task10(void)  
{
   int n[]={0,1,2,3,4, -1}; 
	 int x;
	 
   while(1) 
   { 
      sendByte('P');	
      x = pendSemOS(n, INFINITEOS);	
		  sendByte('p');	
      postSemOS(x+10); 	
    } 
} 
void task11(void)  
{
   int n[]={0,1,2,3,4, -1}; 
	 int x;
	 
   while(1) 
   { 
      sendByte('Q');	
      x = pendSemOS(n, INFINITEOS);	
		  sendByte('q');	
      postSemOS(x+20); 			 
    } 
} 

void task12(void)  
{	
	while(1)
  { 
     sendByte('R');	
     postSemOS(0);   delayTickOS(10);
     postSemOS(1);   delayTickOS(10);
     postSemOS(2);   delayTickOS(10);
     postSemOS(3);   delayTickOS(10); 
     postSemOS(4);   delayTickOS(10); 
  } 
} 


void (*taskName[])(void)={task0,task1,task2,task3,task4,task5,task6,task7,task8,task9,task10,task11, task12};

	
      // ErrorCode : 1- TaskCountOS != TASKSIZE+1
int main(void)  
{
	   char errorCode;
	   int startTaskIndex;
	   int arraySize;
	
	   initializeUART(9600);   // 600 bytes
     NVIC_SetPriority(UART_IRQn,0x0 );	
	
	   arraySize = sizeof(taskName) / sizeof(taskName[0]);
     startTaskIndex = 0;
     errorCode = startOS(taskName, arraySize, startTaskIndex, CLOCKOS, NULL); 
		 sendByte('0'+ errorCode);	// never execute if start successfully
	 
} // main


