
#include "SIZE.h"
#include "OS.h"

#define  DISABLE_INTERRUPT  __asm( "cpsid i		\n" );
#define  ENABLE_INTERRUPT   __asm( "cpsie i		\n" );

typedef struct 
{
   unsigned int  sp;  // sp must be the first element
	 int           pack[PADDING];
	 unsigned int  registerStack[16];
} stackOS;     

stackOS       *CurrentTaskOS;      // the same as SP address
stackOS       *NextTaskOS;         // the same as SP address
stackOS       TaskOS[TASKSIZE+1];  // include idleTaskOS()
int           WaitTickOS[TASKSIZE];

unsigned int  ReadyTableOS;  // bit 1 is ready, priority is bit index
int           CurrentPriorityOS;   // priority <= TASKSIZE - 1, priority begin from 0, priority >= 0
int           TickPerSecondOS;

char          SemNumberTaskOS[TASKSIZE];  // initialized in startOS(), used in pendSemOS(), array index is priority, array value is task's pendding SemNumber(0 ~ 255), one task pends one SEM number.
char          PriorityOwnEventOS[TASKSIZE];     // array index is priority, 0 <= priority <=  TASKSIZE - 1

      // original local variables
int  highestPriority;     // executeHighestPriorityTaskOS()
int  pack;                // findOptimalPaddingOS()
int  maximum;             // findOptimalPaddingOS()
char pend;                // IsPending()  
char schedule;            // SysTick_Handler()
int  tick;	              // delayTimeOS(), lowPowerModeOS()
int  ready;               // lowPowerModeOS()
char bit;	                // checkReadyTableSetBitOS()
unsigned int priorityBit; // checkReadyTableSetBitOS()
		
extern void sendByte(char);
extern void print32bits(unsigned  int );


void initializeSysTickOS(unsigned int clock)
{
	  SystickLoadRegisterOS = clock -1;
	  SystickCurrentValueRegisterOS = 0x0;
	  SystickControlRegisterOS = (1<<0) | (1<<1) | (1<<2); // enable, interrupt, system clock
}


void setHandlerPriorityOS(void)
{
	  unsigned int lowest = 0xff;
	  unsigned int highest = 0x0;
	
	  HandlerPriorityRegisterOS |= lowest  << 16;
	  HandlerPriorityRegisterOS |= highest << 24;
}


void setReadyTableOS(int priority)  
{
	DISABLE_INTERRUPT;
	  ReadyTableOS |= ( 1<< priority );
	ENABLE_INTERRUPT;
}


void clearReadyTableOS(int priority)
{
	ENABLE_INTERRUPT;
	  ReadyTableOS &=  ~( 1<< priority );	
	ENABLE_INTERRUPT;
}


                  // Table must not be zero
__asm  int findLeastBitOS(unsigned int Table)
{
	MOVS R3, #0
	
loop
	LSRS R0,R0,#1
	BCS  return
	ADDS  R3, R3, #1
	CMP  R3, #32
	BLT  loop

return
	MOV  R0, R3  
	BX   LR
}



__asm  int interruptNumberOS()
{
  MRS  R0, IPSR
	BX   LR	
}

__asm void PendSV_Handler(void)
{	
//PUSH 
	MRS	R0, PSP  
	SUBS R0, #32
	STMIA R0!,{R4-R7} 
	MOV	R4, R8
	MOV	R5, R9
	MOV	R6, R10
	MOV	R7, R11
	STMIA R0!,{R4-R7}   
	SUBS R0, #32       
	IMPORT  CurrentTaskOS  
	LDR	R2, =CurrentTaskOS
	LDR	R1, [R2]         
	STR	R0, [R1]        
	
//POP	
	IMPORT  NextTaskOS   
	LDR	R2, =NextTaskOS
	LDR	R1, [R2]
	LDR	R0, [R1]        
	LDMIA R0!,{R4-R7}  
	LDMIA R0!,{R1-R3}  
	MOV	R8, R1
	MOV	R9, R2
	MOV	R10, R3
	LDMIA R0!,{R1}	
	MOV	R11, R1        

	MSR	PSP, R0      

	LDR R3, =0xFFFFFFFD  

	BX	R3       
}

__asm void setPSPOS(unsigned int topPointer)
{
	  PRESERVE8
	  THUMB
	
    MSR  PSP, R0
	  BX   LR	
}


__asm void setCONTROLOS(unsigned int usePSP)
{
		PRESERVE8
	  THUMB
	
    MSR  CONTROL, R0
	  ISB
	  BX   LR	
}

                     // 	need set ReadyTableOS, do not need TaskOS[ CurrentPriorityOS ].waitTick = 0;
void executeHighestPriorityTaskOS(void)
{	
	          // task that owns event does not need to schedule. it means the task owns the highest priority to occupy CPU
	  if ( ( PriorityOwnEventOS[CurrentPriorityOS] < 1 ) || ( CurrentPriorityOS == (int)TASKSIZE ) )  // CurrentPriorityOS  that own events does not switch context( with highest priority )
		{
        highestPriority =  findLeastBitOS(ReadyTableOS);

        if(  highestPriority != CurrentPriorityOS   )
        {  
					  DISABLE_INTERRUPT;
	      		        //  context Switch
	             CurrentTaskOS = &TaskOS[CurrentPriorityOS];
               NextTaskOS = &TaskOS[highestPriority];
	             CurrentPriorityOS = highestPriority;	    // change CurrentPriorityOS							
               InterruptControlRegisterOS = 1 << 28;
					  ENABLE_INTERRUPT;
        }
			 
		} // if
			 
}



 void initializeTaskOS( void (*handler)(void), int priority)
{
		int i;
	
    DISABLE_INTERRUPT;				 
     setReadyTableOS(priority );    

	   TaskOS[priority].sp = (unsigned int)( &TaskOS[priority].registerStack[0] );  
     TaskOS[priority].registerStack[15] = 0x01000000;                  
		 TaskOS[priority].registerStack[14] = (unsigned int)handler; 
    ENABLE_INTERRUPT;	

		for( i=0; i<(int)PADDING; i++ )
    {
			 DISABLE_INTERRUPT;	
        TaskOS[priority].pack[i] = 0xf000000f;
			 ENABLE_INTERRUPT;	
    }				 
} 



void idleTaskOS(void)
{
     while(1) 
		 {

		 }
} 



          // ReadyTable is initialized
char startOS(void (*taskName[])(void), int arraySize, int startPriority, int clock)
{ 
	   int         i;

     if ( arraySize !=  (int)TASKSIZE  )  
		 { 	
			   return 1;     // error will stop OS
		 }
		 
     if ( arraySize >= 32  )   // maximum TASKSIZE is 31
		 { 	
			   return 2;     // error will stop OS
		 }		
		       // initialize user's tasks
		 for ( i=0; i<TASKSIZE; i++) // idleTaskOS's priority =  TASKSIZE
		 {
			 DISABLE_INTERRUPT;
			   SemNumberTaskOS[i] = (char)-1;				 
			 	 PriorityOwnEventOS[i] = 0;
	       WaitTickOS[i] = 0;		
			 ENABLE_INTERRUPT;
			   initializeTaskOS(taskName[i], i);  
		 }
		
		 initializeTaskOS(idleTaskOS, TASKSIZE);  // create system's idleTaskOS()	 
	   initializeSysTickOS(clock);
		 setHandlerPriorityOS();	
		 
    DISABLE_INTERRUPT;
		 TickPerSecondOS = SystemCoreClock / clock;
     CurrentPriorityOS = startPriority;		 
     CurrentTaskOS = &TaskOS[CurrentPriorityOS]; 
		ENABLE_INTERRUPT;

		 setPSPOS((int)&CurrentTaskOS->registerStack[15] + 4);
     setCONTROLOS(0x02);         // use PSP
		 
		 taskName[CurrentPriorityOS]();
	 
		 return 0;   // never reach if successful
	 
 }//  st artOS

  

int findOptimalPaddingOS(void) 
{
	  int i;
    int k;
	
	  maximum = -1;
	
	  for ( i=0; i< TASKSIZE; i++ )
	  {
        k = 0;
        while( (TaskOS[i].pack[k] == 0xf000000f) && (k < PADDING) )
        {
              k++;
        }

        pack = (int)PADDING - k;                                

		    if (  pack > maximum )
		    {
			      maximum = pack;       
		    }		

	    } // for
				
      return maximum;
}



char IsStartPending(void)
{
	  int  i;  

		pend = 0;
	
		for( i = 0; i < TASKSIZE; i++  )   
    {
		 	  if(  SemNumberTaskOS[i] != (char)-1 )
				{
            pend = 1;
					  break;
				}				
		 }

     return pend;		 
}


             // posting task does not know whick task pend the semNumber
             // multiTask pend the same SemNumber is permissible
void postSemOS(char semNumber)
{
	  int i;     // priority

	  if (  semNumber != (char)-1 )
		{
	
	     DISABLE_INTERRUPT;	
				 PriorityOwnEventOS[CurrentPriorityOS] = 0;  // used in executeHighestPriorityTaskOS()			
	       SemNumberTaskOS[CurrentPriorityOS] = (char)-1;   // does not pend Sem
	     ENABLE_INTERRUPT;	
			
			   if(  IsStartPending() )
				 {			
	 	        for ( i=0; i < TASKSIZE; i++ )
            {
							  if(   SemNumberTaskOS[i] == semNumber  )  // resume all task pendding this number
							  {	
	                  DISABLE_INTERRUPT;									 
	                    SemNumberTaskOS[i] = (char)-1; 		// delete sem number
			                PriorityOwnEventOS[i] = 1;
								      WaitTickOS[i] = 0;
		                  setReadyTableOS(i);  //  the pending task is ready, regardless the waitTick value
								 	  ENABLE_INTERRUPT;	
						    }
								
            } // for
						
				 } // if(  IsStartPending() )
					
	  } //   if (  semNumber >= 0 )
		if( interruptNumberOS() == 0 )
		{			
	      executeHighestPriorityTaskOS();	// successful
		}
}



void pauseTaskOS(int timeout)
{	
     DISABLE_INTERRUPT;				
	    PriorityOwnEventOS[CurrentPriorityOS] = 0;	   // current task does not own event
	    WaitTickOS[CurrentPriorityOS] = timeout;
	 	  clearReadyTableOS(CurrentPriorityOS );
     ENABLE_INTERRUPT;		
}


                    //  timeout is tick number 
void pendSemOS(char semNumber, int timeout)
{ 
	   if ( interruptNumberOS() == 0 )
		 {		
       DISABLE_INTERRUPT;			 
			   SemNumberTaskOS[CurrentPriorityOS] = semNumber; 
		   ENABLE_INTERRUPT;	
		 
		     pauseTaskOS(timeout);
		 
         executeHighestPriorityTaskOS();
	   }		 
}



void SysTick_Handler(void)
{
     int  i;

		 schedule=0; 
		            // one Tick Passed and set ready
     for( i=0; i<TASKSIZE; i++ )   // i is task's priority
     {
				  if ( WaitTickOS[i] >= 1 )		// waitTick < 0 can not be ready(infinite timeout), only acquiring event can let the  pendding task ready   
          {
							 DISABLE_INTERRUPT;
				          WaitTickOS[i]--;	
							 ENABLE_INTERRUPT;
          } 
						 
				  if ( WaitTickOS[i] == 0  )	
          {	
					        setReadyTableOS(i);							 
                DISABLE_INTERRUPT;							 
							    schedule = 1;
							  ENABLE_INTERRUPT;
          } 						 
								
     } // for
				
           // system can schedule tasks due to System Tick
			if ( schedule )
			{
  	       executeHighestPriorityTaskOS();		
			}
}



void delayTickOS(int tick)
{
		if( interruptNumberOS() == 0 )
		{	
        pauseTaskOS(tick);		 
        executeHighestPriorityTaskOS();	
		}
} 



void delayTimeOS( int hour, int minute, int second, int  mS)
{
		if( interruptNumberOS() == 0 )
		{	
	     if ( ( hour >= 0 ) && (  minute >= 0 ) && ( second >= 0 ) && ( mS >= 0 ) )
		   {
	         tick = TickPerSecondOS * ( hour * 3600 + minute * 60 + second );
	         tick += TickPerSecondOS * mS / 1000; 
			 
			     if ( (TickPerSecondOS * mS % 1000) > 500 )
				   {
					    tick++; 
				   }	
				 
			     if ( tick < 1 )
				   {
					    tick = 1; 
				   }
					 
	         pauseTaskOS(tick);
		   }
		 
       executeHighestPriorityTaskOS();	 
	 }
		 
} 


      // return 1 or 0 only
char checkReadyTableSetBitOS(int priority)
{
    priorityBit = 0x0;
	
	  priorityBit |=  ( 1<< priority );
	  priorityBit &= ReadyTableOS;
	
	  bit =1;
	  if ( priorityBit == 0x0 )    
	  {
		    bit = 0;
	  }
	 
	  return bit;
}


void queryReadyTableOS(char* result)
{
	  int i;
	
	  for (i=0; i<TASKSIZE; i++)
	  {
		    result[i] = checkReadyTableSetBitOS(i);
	  }
}

int lowPowerModeOS(int *next)
{
	  int i;
	
	  ready = 0; 
	  tick = 999999;
	  *next = -1;
	
	  for (i=0; i<TASKSIZE; i++)
	  {
		    if ( checkReadyTableSetBitOS(i) )
				{
					  ready++;
				}
	  }

    if ( ready == 1 )
		{
	     for (i=0; i<TASKSIZE; i++)
	     {
			     if ( (i != CurrentPriorityOS) && (WaitTickOS[i] < tick) )
					 {
							 tick = WaitTickOS[i];
						 	 *next = i;
					 }
		   }
		}		

    tick = ( (tick > 2) && (tick != 999999) )	? tick - 1 : -1;
	
		return  tick;
}





