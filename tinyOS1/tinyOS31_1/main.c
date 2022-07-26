#include <LPC11xx.h>
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
	
  	while ( !(LPC_UART->LSR & THRE) ); 
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
{	 while(1)   {  sendByte('0');  pendSemOS(0, INFINITEOS  ); 	 postSemOS(1);  }   } 
void task1(void)  
{	 while(1)   {  sendByte('1');	 pendSemOS(1, INFINITEOS  );	 postSemOS(2);   }   } 
void task2(void)  
{	 while(1)   {  sendByte('2');	 pendSemOS(2, INFINITEOS  );	 postSemOS(3);   }   } 
void task3(void)  
{	 while(1)   {  sendByte('3');	 pendSemOS(3, INFINITEOS  );	 postSemOS(4); 	  }   } 
void task4(void)  
{	 while(1)   {  sendByte('4');	 pendSemOS(4, INFINITEOS  );	 postSemOS(5);   }   } 
void task5(void)  
{	 while(1)   {  sendByte('5');	 pendSemOS(5, INFINITEOS  );	 postSemOS(6);   }   } 
void task6(void)  
{	 while(1)   {  sendByte('6');	 pendSemOS(6, INFINITEOS  );	 postSemOS(7); 	  }   } 
void task7(void)  
{	 while(1)   {  sendByte('7');	 pendSemOS(7, INFINITEOS  );	 postSemOS(8);   }   } 
void task8(void)  
{	 while(1)   {  sendByte('8');	 pendSemOS(8, INFINITEOS  );	 postSemOS(9);   }   } 
void task9(void)  
{	 while(1)   {  sendByte('9');	 pendSemOS(9, INFINITEOS  );	 postSemOS(10); 	  }   } 
void task10(void)  
{	 while(1)   {  sendByte('a');	 pendSemOS(10, INFINITEOS  );	 postSemOS(11);    }   } 
void task11(void)  
{	 while(1)   {  sendByte('b');	 pendSemOS(11, INFINITEOS  );	 postSemOS(12); 	  }   } 
void task12(void)  
{	 while(1)   {  sendByte('c');	 pendSemOS(12, INFINITEOS  );	 postSemOS(13); 	  }   } 
void task13(void)  
{	 while(1)   {  sendByte('d');	 pendSemOS(13, INFINITEOS  );	 postSemOS(14); 	   }   } 
void task14(void)  
{	 while(1)   {  sendByte('e');	 pendSemOS(14, INFINITEOS  );	 postSemOS(15); 	  }   } 
void task15(void)  
{	 while(1)   {  sendByte('f');	 pendSemOS(15, INFINITEOS  );	 postSemOS(16);   }   } 
void task16(void)  
{	 while(1)   {  sendByte('g');	 pendSemOS(16, INFINITEOS  );	 postSemOS(17); 	  }   } 
void task17(void)  
{	 while(1)   {  sendByte('h');	 pendSemOS(17, INFINITEOS  );	 postSemOS(18); 	  }   } 
void task18(void)  
{	 while(1)   {  sendByte('i');	 pendSemOS(18, INFINITEOS  );	 postSemOS(19);    }   } 
void task19(void)  
{	 while(1)   {  sendByte('j');	 pendSemOS(19, INFINITEOS  );	 postSemOS(20); 	   }   } 
void task20(void)  
{	 while(1)   {  sendByte('k');	 pendSemOS(20, INFINITEOS  );	 postSemOS(21);   }   } 
void task21(void)  
{	 while(1)   {  sendByte('l');	 pendSemOS(21, INFINITEOS  );	 postSemOS(22); 	  }   } 
void task22(void)  
{	 while(1)   {  sendByte('m');	 pendSemOS(22, INFINITEOS  );	 postSemOS(23);   }   } 
void task23(void)  
{	 while(1)   {  sendByte('n');	 pendSemOS(23, INFINITEOS  );	 postSemOS(24);   }   } 
void task24(void)  
{	 while(1)   {  sendByte('o');	 pendSemOS(24, INFINITEOS  );	 postSemOS(25);    }   } 
void task25(void)  
{	 while(1)   {  sendByte('p');  pendSemOS(25, INFINITEOS  );	 postSemOS(26);    }   } 
void task26(void)  
{	 while(1)   {  sendByte('q');	 pendSemOS(26, INFINITEOS  );	 postSemOS(27); 	   }   } 
void task27(void)  
{	 while(1)   {  sendByte('r');	 pendSemOS(27, INFINITEOS  );	 postSemOS(28);    }   } 
void task28(void)  
{	 while(1)   {  sendByte('s');	 pendSemOS(28, INFINITEOS  );	 postSemOS(29);    }   } 
void task29(void)  
{	 while(1)   {  sendByte('t');	 pendSemOS(29, INFINITEOS  );	 postSemOS(30); 	   }   } 
void task30(void)  
{	 
  while(1) 
  {  
      sendByte('u');	 postSemOS(0); 
		  delayTimeOS(0,0,0, 400);  
	//	  print32bits(findOptimalPaddingOS());
  }
} 


void (*taskName[])(void)={task0,task1,task2,task3,task4,task5,task6,task7,task8,task9,task10,task11,task12,task13,task14,task15,task16,task17,task18,task19,
      task20,task21,task22,task23,task24,task25,task26,task27,task28,task29,task30};

			

      // ErrorCode : 1- TaskCountOS != TASKSIZE+1
int main(void)  
{
	   char errorCode;
	   int startTaskIndex;
	   int arraySize;
	
	   initializeUART(9600);   // 600 bytes
	   arraySize = sizeof(taskName) / sizeof(taskName[0]);
     startTaskIndex = 0;
     errorCode = startOS(taskName, arraySize, startTaskIndex, CLOCKOS);  // create idleTaskOS()
		 sendByte('0'+ errorCode);	// never execute if start successfully 
	
} // main




