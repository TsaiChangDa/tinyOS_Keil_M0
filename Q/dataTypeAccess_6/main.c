
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

int      p = 0;
int      a[] = {1, 2, 3};
char     b[] = {4, 5};
float    c[] = {0.6};
int**    pint2;
void**   pvoid;
int*     pint;
char*    pchar;
float*   pfloat;

void task0(void)  
{
     while(1)
     {
		      sendByte('A');
		 	    postQOS(0, a);
		 	    postQOS(0, b); 
		 	    postQOS(0, c); 
		      delayTickOS(10);
     }
}

void task1(void)  
{	
     while(1)
     {
		      sendByte('B');
			    p++;
		 	    postQOS(1, &p);
		      delayTickOS(10);
     }
}
 
void task2(void)  
{	 
	 int   number[]={1, 0, -1};
   int items;
	 
   while(1)
   {
		   sendByte('C');
		   pint2 = pendQOS(number, 1, &items, INFINITEOS );   // Q1
		   sendByte('c');		 
			 sendByte('0' + qReadyNumberOS(pint2) );
		 	 sendByte('0' + items );     
		 	 print32bits( (unsigned int)  **pint2 );
		 	 print32bits( (unsigned int)  pint2 );		
		 
//-----------------------------------------------------------
		 
		 	 sendByte('X');	   // Q1
		   pint2 = readQOS(1, &items);
		 	 print32bits( (unsigned int)  items );	
		 	 print32bits( (unsigned int)  pint2 );	

		   sendByte('Y');	      // Q0	 
		   pvoid = readQOS(0, &items);
		   pint = *(pvoid + 0 );
		   pchar = *(pvoid + 1 );
		   pfloat = *(pvoid + 2 );	
		 
			 sendByte('0' + items );
		   print32bits( (unsigned int)  pint );
		   print32bits( (unsigned int)  pchar );
		   print32bits( (unsigned int)  pfloat );
			 
			 sendByte('Z');	 
			 sendByte('0' + *pint);                  // 1	
			 sendByte('0' + *(pint+1));              // 2	
			 sendByte('0' + *(pint+2));              // 3	
			 
			 sendByte('0' + *pchar);                 // 4	
			 sendByte('0' + *(pchar+1));             // 5	
			 sendByte('0' + (int)(*pfloat * 10.0));  // 6	

    }//while
}

void (*taskName[])(void)={task0, task1, task2};


      // ErrorCode : 1- TaskCountOS != TASKSIZE+1
      // ErrorCode : 2- MEMORYPOOLBYTES too little
int main(void)  
{
	   char errorCode;
	   int startTaskIndex;
	   int arraySize;
	
	   initializeUART(9600);   // 600 bytes

		 sendByte('a');	
		 print32bits( (unsigned int)  a );
		 sendByte('b');	
		 print32bits( (unsigned int)  b );
		 sendByte('c');	
		 print32bits( (unsigned int)  c );
	
	   arraySize = sizeof(taskName) / sizeof(taskName[0]);
     startTaskIndex = 0;
     errorCode = startOS(taskName, arraySize, startTaskIndex, CLOCKOS, NULL); 
		 sendByte('0'+ errorCode);	// never execute if start successfully
	 
} // main




