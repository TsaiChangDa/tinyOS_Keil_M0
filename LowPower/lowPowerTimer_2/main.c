
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

void initializeLowPowerTimer(void)
{
		 LPC_SYSCON->SYSAHBCLKCTRL |=  (1<<9); // enable LPC_TMR32B0 clock
		 LPC_TMR32B0->PR = 0;
		 LPC_TMR32B0->MR1 = CLOCKOS -1;
		 LPC_TMR32B0->MCR = (1<<3) | (1<<4);   // interrupt and reset by MR1
	   NVIC_EnableIRQ(TIMER_32_0_IRQn);
}

void TIMER32_0_IRQHandler(void)  // simulate low-power timer
{
	  extern void SysTick_Handler(void);
	
		sendByte('X');
	  LPC_TMR32B0->IR = (1<<1);  // clear interrupt flag
		SysTick_Handler();
}

void stopPeripheralClock(void)
{
		SystickControlRegisterOS = ~(1<<0) & ~(1<<1) & ~(1<<2);  // disable Systen Tick timer
		LPC_SYSCON->SYSAHBCLKCTRL |=  (1<<9); // enable LPC_TMR32B0 clock
		LPC_TMR32B0->TCR = (1<<1);   // reset low-power counter
		LPC_TMR32B0->TCR = (1<<0);   // enable low-power counter
	
	// stop peripheral clock
	
    sendByte('1');
}

void restartPeripheralClock(void)
{
		LPC_TMR32B0->TCR &= ~(1<<0);   // disable low-power counter
		LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<9); // disable LPC_TMR32B0 clock				
				
	  SystickLoadRegisterOS = CLOCKOS -1;
	  SystickCurrentValueRegisterOS = 0x0;
	  SystickControlRegisterOS = (1<<0) | (1<<1) | (1<<2); // enable Systen Tick timer
	
	 // restart peripheral clock
	
    sendByte('2');
}

void task0(void)  
{			 
	  while(1)
    { 
        sendByte('A');
			  delayTickOS( 37 );		
    } 
} 


void task1(void)  
{	
    while(1) 
    { 
        sendByte('B');	
				delayTickOS( 31 );
    }  
} 

void task2(void)  
{			 
	  while(1)
    { 
        sendByte('C');	
			  delayTickOS( 23 );		
    } 
} 


void task3(void)  
{	
    while(1) 
    { 
        sendByte('D');			
			  delayTickOS( 17 );
    }  
} 

void (*taskName[])(void)={task0,task1, task2,task3};

	
      // ErrorCode : 1- TaskCountOS != TASKSIZE+1
int main(void)  
{
	   char errorCode;
	   int startTaskIndex;
	   int arraySize;
	
	   initializeUART(9600);   // 600 bytes
	   initializeLowPowerTimer();
	
	   arraySize = sizeof(taskName) / sizeof(taskName[0]);
     startTaskIndex = 0;
     errorCode = startOS(taskName,arraySize,startTaskIndex,NULL,stopPeripheralClock,restartPeripheralClock); 
		 sendByte('0'+ errorCode);	// never execute if start successfully
	 
} // main


