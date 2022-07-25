
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

int  a[] = {5};
int  b[] = {6};
int  c[] = {7};
int  d[] = {8};
int  e[] = {9};

int    n0[]={0, -1};
int    n1[]={1, -1};
int**  pint2;

void task0(void)   
{
	  while(1)
    {
		     sendByte('A');
			 	 postQOS(0, a);
				 postQOS(0, b);
				 delayTickOS(16);
    }
} 

void task1(void)   
{ 
	  while(1)
    {
		     sendByte('B');
			 	 postQOS(1, a);
				 postQOS(1, b);
				 postQOS(1, c);
				 postQOS(1, d);
				 postQOS(1, e);
				 delayTickOS(10);
    }
} 

void task2(void)   
{
	  int items;
	
	  while(1)
    {
		     sendByte('C');
		  	 pint2 = pendQOS(n0, 1, &items, INFINITEOS );
			   sendByte('c');
		     sendByte('0'+ queryRemainItemsOS(0) );   
    }
} 

void task3(void)  
{
	  int  i;
	  int  items;
	
	  while(1)
    {
		    sendByte('D');
		    pint2 = pendQOS(n1, 1, &items, INFINITEOS );	
			  sendByte('d');
        sendByte('0'+ queryRemainItemsOS(1) );

			  sendByte('@');			
			  for(i=0; i<items; i++)
				{
					   sendByte('0'+ **(pint2 + i) );
				}
    }
} 

void (*taskName[])(void)={task0,task1,task2,task3};

      // ErrorCode : 1- TaskCountOS != TASKSIZE+1
int main(void)  
{
	   char errorCode;
	   int startTaskIndex;
	   int arraySize;
	
	   initializeUART(9600);   // 600 bytes
	
	   arraySize = sizeof(taskName) / sizeof(taskName[0]);
     startTaskIndex = 0;
     errorCode = startOS(taskName, arraySize, startTaskIndex, CLOCKOS, NULL); 
		 sendByte('0'+ errorCode);	// never execute if start successfully
	 
} // main




