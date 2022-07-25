
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

unsigned int postFlag0 = 0x000000ff;  // set flag0
unsigned int postFlag1 = 0x0000ff00;  // set flag1
unsigned int postFlag2 = 0x000000ee;  // flag0 is cleared to 0x11
unsigned int postFlag3 = 0x00008c00;  // flag1 is clear to 0x7300
unsigned int postFlag4 = 0x00000001;  // flag0 is cleared from 0x11 to 0x10

unsigned int pendFlag1 = 0x00000008;  // ANY  flag0
unsigned int pendFlag2 = 0x00000400;  // ANY  flag1
unsigned int pendFlag3 = 0x00000011;  // ALL  flag0
unsigned int pendFlag4 = 0x00007000;  // ALL  flag1
unsigned int pendFlag5 = 0x00000010;  // ALL  flag0

unsigned int writeMail[3];
unsigned int writeWord;
int  readyNo;

void task0(void)   
{
	  int i;
    int number[] = {0 , -1};
	  unsigned int *readAddress;
	
	  while(1)
    {
		    sendByte('A');
        readAddress = pendMailOS(number, &readyNo, 0, INFINITEOS);
			  for(i=0; i<3;i++)
				{
				    print32bits( (unsigned int) *(readAddress+i) );
				}
			 	 postFlagOS(0,postFlag0 , FLAG_SET);
    }
} 

void task1(void)   
{ 
    int number[] = {0 , -1};
		
	  while(1)
    {
		    sendByte('B');
			  pendFlagOS(number, pendFlag1, FLAG_MATCH_ANY, INFINITEOS );
				postFlagOS(1,postFlag1 , FLAG_SET);
    }
} 

void task2(void)   
{
    int number[] = {1 , -1};
		
	  while(1)
    {
		    sendByte('C');
			  pendFlagOS(number, pendFlag2, FLAG_MATCH_ANY, INFINITEOS );
				postFlagOS(0,postFlag2 , FLAG_CLEAR);
    }
} 

void task3(void)  
{
    int number[] = {0 , -1};
		
	  while(1)
    {
		    sendByte('D');
			  pendFlagOS(number, pendFlag3, FLAG_MATCH_ALL, INFINITEOS );
				postFlagOS(1,postFlag3 , FLAG_CLEAR);
    }
} 

void task4(void)   
{
    int number[] = {1 , -1};
		
	  while(1)
    {
		    sendByte('E');
			  pendFlagOS(number, pendFlag4, FLAG_MATCH_ALL, INFINITEOS );
				postFlagOS(0,postFlag4 , FLAG_CLEAR);
    }
} 

void task5(void)   
{
    int number[] = {0 , -1};
		
	 while(1)
   {
		    sendByte('F');
			  pendFlagOS(number, pendFlag5, FLAG_MATCH_ALL, INFINITEOS );
		    writeWord = 0xffffffff;
				postMailOS(1, &writeWord);
   }
} 

void task6(void)   
{
    int number[] = {1 , -1};	
	  unsigned int *readAddress;
	
	  while(1)
    {
		    sendByte('G');
			  readAddress = pendMailOS(number, &readyNo, 0, INFINITEOS);
        print32bits( *readAddress );	
				postMailOS(2, 0x0);  // don't send message
    }
} 

void task7(void)  
{
    int number[] = {2 , -1};
		
	 while(1)
   {
		    sendByte('H');
			  pendMailOS(number, &readyNo, 0, INFINITEOS);
   }
} 

void task8(void)   
{
	
	 while(1)
   {
		    sendByte('I');
		 
		    writeMail[0]= 0xaaaaaaaa;
		    writeMail[1]= 0xbbbbbbbb;
		    writeMail[2]= 0xcccccccc;

	      postMailOS(0, writeMail);	
		 
		 		delayTimeOS(0,0,0,800);	
   }
} 

void (*taskName[])(void)={task0,task1,task2,task3,task4,task5,task6,task7,task8};

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




