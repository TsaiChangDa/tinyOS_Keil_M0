
#include "SIZE.h"
#include "OS.h"

#define  TABLELENGTH      ( ( TASKSIZE + 1 ) / 33   + 1 )
#define  BULKBYTES         sizeof(long double)
#define  GLOBALBULKLENGTH ( MEMORYPOOLBYTES % BULKBYTES ? MEMORYPOOLBYTES / BULKBYTES + 1 : MEMORYPOOLBYTES / BULKBYTES )	
#define  FREEBULKLENGTH   ( GLOBALBULKLENGTH ? GLOBALBULKLENGTH / 33   + 1 : 0 )
#define  qBYTES            BULKBYTES * ( QBYTES % BULKBYTES ? QBYTES / BULKBYTES + 1 :  QBYTES / BULKBYTES )
#define  QLENGTH          ( (qBYTES + BULKBYTES) / sizeof(void*) )

          // inline.c
extern int findLeastBitOS(unsigned int);
extern int interruptNumberOS(void);
extern void setPSPOS(unsigned int);
extern void setCONTROLOS(unsigned int);
extern void PendSV_Handler(void);

typedef struct
{
   void**  q;    
	 int     inIndex;      // the only entry(array index) to push	
   int     outIndex;
	 int     items;
}qbodyOS;	


typedef struct
{
   int            priority;         // pending task
   int           *numberArray;	
	 int            readyNumber;   
   char           eventType;	
	 unsigned int   privateFlag;      // flag
} eventOS;

unsigned int* CurrentTaskOS;      // address of sp(the entry of CPU registers stored in ram stack)
unsigned int* NextTaskOS;         // address of sp 
int           CurrentPriorityOS;   // 0 <= task priority <= TASKSIZE - 1
int           TickPerSecondOS;
int           WaitTickOS[TASKSIZE];     // exclude idleTaskOS()
int           ScheduleISROS = 0;
unsigned int  ReadyTableOS[TABLELENGTH];  // bit index is priority
char          ErrorPendSizeOS = 0;

unsigned int* TaskSpPointerOS[TASKSIZE+1];  // include idleTaskOS()
unsigned int  TaskStackOS[STACKRAMBYTES/4];  // include all tasks and idleTaskOS()
int           PackSizeOS[TASKSIZE+1];
int           AutoPackItemsOS;

unsigned int  PriorityOwnEventOS[TABLELENGTH+4];
eventOS       EventNumberTaskOS[PENDSIZE+4];


void*         ReceiveMessageOS[MAILSIZE+4];	 


int           MutexOwnerOS[MUTEXSIZE+4];   // priority of mutex owner


unsigned int  PublicFlagOS[FLAGSIZE+4];

unsigned int  FlagAllOrAnyOS[TABLELENGTH+4];  // bit value 1 means private flag must match all the bits, bit value 0 means private flag could match only one bit, priority is bit index

long double   PoolOS[GLOBALBULKLENGTH+4];
unsigned int  FreeBulkNoOS[FREEBULKLENGTH+4];
int           StartBulkNoOS[GETMEMORYSIZE+4];  
int           DangerBytesOS[GETMEMORYSIZE+4];
char          LeakNoOS[GETMEMORYSIZE+4];
char          LackNoOS[GETMEMORYSIZE+4];
int           LeakNoAllOS[GETMEMORYSIZE+4];
int           LackNoAllOS[GETMEMORYSIZE+4];
void*         RelyMargeDangerOS[GETMEMORYSIZE+4][3];
void*         MallocAddressOS;

int*          DelayUntilOS[UNTILSIZE+4][2];       // condition
int           DelayUntilPriorityOS[UNTILSIZE+4];  // priority 

qbodyOS       QBodyOS[QSIZE+4];
void*         PendQRetrieveOS[QSIZE+4][QLENGTH];
int           ItemsPendQOS[QSIZE+4];
char          OverflowPendQOS[QSIZE+4];

char          SleepPendQOS[QSIZE+4];
char          QOverflowOS[QSIZE+4];
int           CountTickOS;
int           TaskClockOS[TASKSIZE+1][2];   // [0]- before task   [1]- after task
unsigned int  TaskExecuteClockOS[TASKSIZE+1];  // clock elapsed
int           TaskLoadOS[TASKSIZE];       // %

/*****************************************************************/
/*                      kernel                                   */
/*****************************************************************/

void* mallocOS(int);                  // called by initializeEventOS()
char  checkDelayUntilOS(void);        // called by SysTick_Handler()
void  assignPaddingSpOS(void);        // called by initializeTaskOS()
void  setTableOS(unsigned int*, int); // called by initializeTaskOS()
void  idleTaskOS(void);               // used   by initializeTaskOS()

#if   defined  CM0 

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
	
	  unsigned int priorityPendSV  = lowest  << 16;
	  unsigned int prioritySysTick = highest << 24;
	
	  HandlerPriorityRegisterOS |= priorityPendSV;
	  HandlerPriorityRegisterOS |= prioritySysTick;
}


void initializeTaskOS( void (*taskName[])(void) )
{
	  int i;
	  int k;
	  unsigned int *spAddr;

	  assignPaddingSpOS();
	
		for ( i=0; i <= TASKSIZE; i++)  // idleTaskOS's priority =  TASKSIZE
		{
			  setTableOS( ReadyTableOS, i );  
			
			 DISABLE_INTERRUPT;
			  TaskExecuteClockOS[i] = 0x0;			
				 
        spAddr =  TaskSpPointerOS[i];

			  *spAddr = (unsigned int) ((int)spAddr + (PaddingOS[i]+1)*WORDSIZE );  // the entry of CPU registers of priority i in stack
			
        *(spAddr + PaddingOS[i] + 16) = 0x01000000;                  // registerStack[15] to xPSR       

				if( i != TASKSIZE )
				{
				    *(spAddr + PaddingOS[i] + 15) = (unsigned int)taskName[i];   // registerStack[14] to PC
				    WaitTickOS[i] = 0; 
				}
				else
				{
				    *(spAddr + PaddingOS[i] + 15) = (unsigned int)idleTaskOS;						 
				}
					 
			  for( k=0; k<PaddingOS[i]; k++ )
        {
            *(spAddr + 1 + k) = 0xf000000f;        // pack[i]
        }	
		  ENABLE_INTERRUPT;	
			 
	 } // for
} 

#endif



void setTableOS(unsigned int* Table, int priority)  
{
	  int  index;   // active priority = 32 * IndexOS + BitOS
	  char bit;	
	
	  index = priority / 32;
	  bit = priority % 32;	
	 DISABLE_INTERRUPT;
	  Table[index] |= ( 1<< bit );
	 ENABLE_INTERRUPT; 
}

void clearTableOS(unsigned int* Table, int priority)  
{
	  int  index;  
	  char bit;	
	
	  index = priority / 32;
	  bit = priority % 32;	
	 DISABLE_INTERRUPT;
	  Table[index]  &=  ~( 1<< bit );	
	 ENABLE_INTERRUPT; 
}


      // return 1 or 0 only
char checkSetBitOS(unsigned int  *Table, int priority)
{
    unsigned int priorityBit = 0x0;
	  int  index;	
	  char bit;	

	  index = priority / 32;	
	  bit = priority % 32;	
	  priorityBit |=  ( 1<< bit );
	  priorityBit &= Table[index];
	  if ( priorityBit == 0x0 )    
	  {
		    return 0;
	  }
	 
	  return 1;
}


void currentExecuteClockOS(void)
{
	  int  i;
    int  clockValue;
	  int  clockDifference;
		                         // Clock  downcount
   	if ( TaskClockOS[CurrentPriorityOS][0] >= TaskClockOS[CurrentPriorityOS][1] )
		{
			  clockValue = TaskClockOS[CurrentPriorityOS][0] - TaskClockOS[CurrentPriorityOS][1];
		}
		else
		{                 
			  clockValue = (int)clockOS  - TaskClockOS[CurrentPriorityOS][1] + TaskClockOS[CurrentPriorityOS][0];
		}		
		
		if (  CountTickOS > 0 )
		{
		    clockDifference = (CountTickOS - 1) * (int)clockOS + clockValue;			
		}
		         // DISABLE_INTERRUPT
		else if ( (CountTickOS == 0) && (TaskClockOS[CurrentPriorityOS][1] >= TaskClockOS[CurrentPriorityOS][0]) )
		{
			  clockDifference = (int)clockOS  - TaskClockOS[CurrentPriorityOS][1] + TaskClockOS[CurrentPriorityOS][0];	
		}
		else
		{
			  clockDifference = clockValue;
		}

	 DISABLE_INTERRUPT;
		TaskExecuteClockOS[CurrentPriorityOS] += clockDifference;
	 ENABLE_INTERRUPT; 

		if ( TaskExecuteClockOS[CurrentPriorityOS] > 0xdfffffff )
		{  
			  for (i=0; i<=TASKSIZE; i++)
				{
		       DISABLE_INTERRUPT;
		        TaskExecuteClockOS[i] = 0x0;
	         ENABLE_INTERRUPT;				
				}
				
			 DISABLE_INTERRUPT;
			  CountTickOS = 0;
	     ENABLE_INTERRUPT;	
		}
}


 
void executeHighestPriorityTaskOS(void)  // do not need waitTick
{
    int highestPriority;	
	  int index;
	  int base;
	  int i;	
	  char setBit;

	   setBit = checkSetBitOS(PriorityOwnEventOS, CurrentPriorityOS);
	
	   if( ( !setBit ) || ( CurrentPriorityOS ==  TASKSIZE ) )  // if CurrentPriorityOS owns event, it executes continuously
 		 {
				  index = 0;
		      while (  ReadyTableOS[index] == 0  )
		      {
				         index++;		
		      }

					if( ReadyTableOS[index] != 0 )
					{
              highestPriority =  findLeastBitOS(ReadyTableOS[index]);
				
					    if ( index >= 1 )
					    {
					        base = 0;
	                for ( i = index; i > 0; i-- )
		              {
			               base += 32;
		              }
	                highestPriority += base;
					    }

						        	// executable condition
              if(  highestPriority != CurrentPriorityOS  )
              {  	
						    DISABLE_INTERRUPT;    // after current task
                  TaskClockOS[CurrentPriorityOS][1] = (int)SystickCurrentValueRegisterOS;
								ENABLE_INTERRUPT; 
								
                  currentExecuteClockOS();
					
								 DISABLE_INTERRUPT;
														         // before next task
                  TaskClockOS[highestPriority][0] = (int)SystickCurrentValueRegisterOS;
								  CountTickOS = 0;	
								
								             //  context Switch
								  CurrentTaskOS = TaskSpPointerOS[CurrentPriorityOS];
								  NextTaskOS = TaskSpPointerOS[highestPriority];
	                CurrentPriorityOS = highestPriority;
 							    InterruptControlRegisterOS = 1 << 28;
						    ENABLE_INTERRUPT; 		 	
              }

				  } // if( ReadyTableOS[index] != 0 )
			 
		 } // if( ( !setBit ) || ( CurrentPriorityOS ==  TASKSIZE ) ) 
}



void idleTaskOS(void)
{
     while(1) 
		 { 
		 }
} 



void initializeEventOS(void)
{
		 int          i;
	   void*        address= 0x0;
	
		 for ( i=0; i<TABLELENGTH; i++)
		 {
			  DISABLE_INTERRUPT;		
			   PriorityOwnEventOS[i] = 0x0;
         FlagAllOrAnyOS[i] = 0x0;	
	      ENABLE_INTERRUPT; 				 
		 }	
		 
		 for ( i=0; i<PENDSIZE; i++)
		 {
			 DISABLE_INTERRUPT;	
			   EventNumberTaskOS[i].priority = -1;	
			   EventNumberTaskOS[i].numberArray = 0x0;				
			   EventNumberTaskOS[i].readyNumber = -1;	
			   EventNumberTaskOS[i].eventType = (char)-1;
			   EventNumberTaskOS[i].privateFlag = 0x0;	
       ENABLE_INTERRUPT; 			 
		 }		 

		 for ( i=0; i<MAILSIZE; i++)
		 {
			  DISABLE_INTERRUPT;	
			   ReceiveMessageOS[i] = 0x0; 
			  ENABLE_INTERRUPT; 
		 }
		 
		 for ( i=0; i<FLAGSIZE; i++)
		 { 
			  DISABLE_INTERRUPT;	
			   PublicFlagOS[i] = 0x0; 
			  ENABLE_INTERRUPT;
		 }
		 
		 for ( i=0; i<MUTEXSIZE; i++)
		 {
			  DISABLE_INTERRUPT;
		      MutexOwnerOS[i] = -1;
			  ENABLE_INTERRUPT;
		 }	
	
		 for ( i=0; i< GETMEMORYSIZE; i++) 
		 {
			  DISABLE_INTERRUPT;	
			   StartBulkNoOS[i] =  -1; 
			   LackNoOS[i] = 0;
			   LeakNoOS[i] = 0;		
         LackNoAllOS[i] = 0;
         LeakNoAllOS[i] = 0;			 
				 DangerBytesOS[i] = 0;
				 RelyMargeDangerOS[i][0] = (void*)0x0;  // relyAddress
				 RelyMargeDangerOS[i][1] = (void*)0x0;  // marginAddress
				 RelyMargeDangerOS[i][2] = (void*)0x0;  // dangerAddress					 
			  ENABLE_INTERRUPT; 
		 }
		 	 
                      // initialize memory pool
		 for ( i=0; i<FREEBULKLENGTH; i++ ) // must before getMemoryOS()
		 {
			   DISABLE_INTERRUPT;
				  FreeBulkNoOS[i] = 0xffffffff;    // 1 is available
			   ENABLE_INTERRUPT;
		 }	
 
     for(i=0; i<QSIZE; i++)	  // must after setting FreeBulkNoOS[]
		 {
			    address = mallocOS(qBYTES);
			 
			  DISABLE_INTERRUPT;
	        QBodyOS[i].q = (void**)address;
			    QBodyOS[i].inIndex =0;
				  QBodyOS[i].outIndex =0;
				  QBodyOS[i].items =0;
			 
			    QOverflowOS[i] = 0;
			  ENABLE_INTERRUPT;
		 } 	

		 for ( i=0; i<UNTILSIZE; i++ ) 
		 {
			   DISABLE_INTERRUPT;
				  DelayUntilOS[i][0] = 0x0;			 
				  DelayUntilOS[i][1] = 0x0;		 
				  DelayUntilPriorityOS[i] = -1;
			   ENABLE_INTERRUPT;
		 }

}



int paddingToRamOS(int totalPadding, int taskSize)
{
	  int ramBytes;
	
	  ramBytes = ( totalPadding + CPUREGISTER*(taskSize+1) ) * WORDSIZE;
	
	  return ramBytes;
}



int ramToPaddingOS(int ramBytes, int taskSize)
{
	  int totalPadding;
	
	  totalPadding = ( ramBytes - CPUREGISTER*(taskSize+1)*WORDSIZE ) / (int)WORDSIZE;
	
	  return totalPadding;
}


void assignPaddingSpOS(void)
{
	  int i;
	  int totalPadding;
	  int k;
		int eachTotal;
	  int amount= 0;

		totalPadding =  ramToPaddingOS(STACKRAMBYTES, TASKSIZE);
	  AutoPackItemsOS = (totalPadding - PADDINGIDLE) / (int)TASKSIZE;	
	
	  for(i=0; i<TASKSIZE; i++)
		{
			  amount += PaddingOS[i];
		}
		
		if ( amount == 0 )  // use STACKRAMBYTES and AutoPackItemsOS
		{
	      for(i=0; i<TASKSIZE; i++)
		    {
			      PaddingOS[i] = AutoPackItemsOS;	
		    }
			 
			  PaddingOS[TASKSIZE] = PADDINGIDLE;      // idleTaskOS()			
		}			
		
		for(i=0; i<=TASKSIZE; i++)
		{
	      eachTotal= 0;
			
		    for(k=0; k<i; k++)
		    {
	         eachTotal += PaddingOS[k];
		    }		
				
		    TaskSpPointerOS[i] = (unsigned int*)( (int)TaskStackOS+ (i*CPUREGISTER + eachTotal)*WORDSIZE );
		}		
	
}



char checkStartErrorOS(int arraySize, int startPriority)
{
	    int i;
		  int amount= 0;
	    char errorCode= 0;
	
	    for(i=0; i<=TASKSIZE; i++)
		  {
			    amount += PaddingOS[i];
		  }	
			
      if (  paddingToRamOS(amount, TASKSIZE) > (int)STACKRAMBYTES )
			{
			    errorCode = 1;  	// amount > 0			
			}
	
                      // Q needs MEMORYPOOLBYTES
      if ( MEMORYPOOLBYTES < QSIZE * (qBYTES + BULKBYTES) )  
		  { 	
			    errorCode = 2;     
		  }	
			
      if ( arraySize !=  TASKSIZE  )  
		  { 	
			    errorCode = 3;     // error will stop OS
		  }			
			
      if (  (startPriority >=  TASKSIZE) || (startPriority < 0)  )  
		  { 	
			    errorCode = 4;     
		  }
			
			
	    return errorCode;
}



char startOS(void (*taskName[])(void), int arraySize, int startPriority, int clock)
{ 
	  char         errorCode;
	  unsigned int topStackPointer;
	  // int topStackPointer;

		errorCode = checkStartErrorOS(arraySize, startPriority);

	  if( errorCode )
		{
			  return errorCode;
		}
	
    initializeSysTickOS(clock);
    setHandlerPriorityOS();		
		initializeEventOS();
		initializeTaskOS(taskName);

		TickPerSecondOS = CPUClock / clock;
    CurrentPriorityOS = startPriority;			 

		topStackPointer = (int)TaskSpPointerOS[CurrentPriorityOS+1];
		setPSPOS(topStackPointer);
    setCONTROLOS(0x03);         // use PSP
		
		taskName[startPriority]();
	 
		return 0;   // never reach if start OS successfully 
 }

 

void resumeTaskOS(int priority)
{
	 DISABLE_INTERRUPT;	
	   WaitTickOS[priority] = 0;
	 ENABLE_INTERRUPT;	
		 setTableOS(PriorityOwnEventOS, priority);
		 setTableOS(ReadyTableOS, priority);  // ready the pending task, regardless waitTick		
}



int currentPriorityMapEventIndexOS(char eventType)
{
	   int i;
	   int index= -1;  
 
	       // one task can simutaneously wait(pend function) various event types(Sem, Box, or Flag).    
  	     // one task can wait only one event in a specific even type.		 
	   for (i=0; i<PENDSIZE; i++)
		 {
         if (  ( (EventNumberTaskOS[i].priority == CurrentPriorityOS) && (EventNumberTaskOS[i].eventType == eventType) ) || (EventNumberTaskOS[i].priority == -1)   )
				 {
					    index = i;
					    break;
				 }
 		 }

		 return index;
}



char IsStartPendOS(char eventType)
{
	  int  i;  
    char pend = 0;

		for( i = 0; i < PENDSIZE; i++  )   
    {
		 	  if(  (EventNumberTaskOS[i].priority >= 0) && (EventNumberTaskOS[i].eventType == eventType)  )
				{
            pend = 1;
					  break;
				}				
		 }

     return pend;		 
}


void pauseTaskOS(int tick)
{
	 DISABLE_INTERRUPT;	
	   WaitTickOS[CurrentPriorityOS] = tick;
	 ENABLE_INTERRUPT;	
		 clearTableOS(PriorityOwnEventOS, CurrentPriorityOS);	
	   clearTableOS(ReadyTableOS, CurrentPriorityOS);		
}


char justifyNumberArrayOS(int *array)  // terminating sign in array
{
	   int  previousNumber = -3;
	   int  i;
	   char error;
	
	   i = 0;  										
     while( ( array[i] >= 0) && ( array[i] != previousNumber )  )								 
     { 
			    previousNumber = array[i];
		      i++;
		 } 	

		 error = 1;    //  no terminating sign
     if ( array[i] < 0 )
		 {
          error = 0;
		 }	

     return error;			
}



int readReadyNumberOS(char eventType)
{
		 char setBit;	
		 int  index;	
     int  readyNumber = -1;
	
	   setBit = checkSetBitOS(PriorityOwnEventOS, CurrentPriorityOS);
	
     if ( setBit > 0 )	
		 {
		  	 index = currentPriorityMapEventIndexOS(eventType);
			   readyNumber = EventNumberTaskOS[index].readyNumber;
		 }
		 
	   return  readyNumber;
}


           // -1: timeout    -2: No terminating sign 
int pendCodeOS(int *array,char eventType)
{
	   char  terminate;
	   int   readyNumber;
	
		 terminate = justifyNumberArrayOS(array);      // error  : terminate = 1
		 readyNumber = readReadyNumberOS(eventType);   // timeout : readyNumber = -1
		 if ( terminate == 1 )
		 {
		    readyNumber = -2;  //  No terminating sign 
		 }		
				
     return readyNumber;		
}



void queryReadyTableOS(char* result)
{
	 int i;
	
	 for (i=0; i<TASKSIZE; i++)
	 {
		   result[i] = checkSetBitOS(ReadyTableOS, i);
	 }
}



         //  ISR calls it at the end for preemptive process.
void  endISRpreemptOS(void) 
{
	  if( ScheduleISROS )
		{
			 DISABLE_INTERRUPT;
			  ScheduleISROS--;
			 ENABLE_INTERRUPT;
			
			  executeHighestPriorityTaskOS(); 
		}
}



int nonBlockingCallbackOS(int (*callback)(void))
{
	  int status = -9;
		
	   DISABLE_INTERRUPT;
		  status = callback();
	   ENABLE_INTERRUPT;	
		
	  return status;
}



void SysTick_Handler(void)
{
     int  i;
	   char schedule=0; 
		 char resume;
	
	    CountTickOS++;
	
      for( i=0; i<= TASKSIZE-1; i++ )   // i is task's priority
      {
           if ( WaitTickOS[i] >= 1 )		// be not ready if WaitTickOS[] < 0, only acquiring event can let the infinite waiting task be ready   
           {
							 DISABLE_INTERRUPT;
				          WaitTickOS[i]--;
							 ENABLE_INTERRUPT;
           } 

				   if ( WaitTickOS[i] == 0  )	
           {
							    setTableOS(ReadyTableOS, i);
						    DISABLE_INTERRUPT;
							    schedule = 1;
							  ENABLE_INTERRUPT;
           } 						 
								
      } // for
				
      resume = checkDelayUntilOS();

			if ( resume || schedule || ScheduleISROS)
			{  
				  DISABLE_INTERRUPT;
				    ScheduleISROS--;
				    if( ScheduleISROS < 0 )
					  {
					 	   ScheduleISROS = 0;
					  }
					ENABLE_INTERRUPT;
			
  	        executeHighestPriorityTaskOS();		
		 	}
}


           // errorPendSizeOS
           // 0-- OK,  1-- pend function too much,  3-- PENDSIZE too large
char errorPendSizeOS(void)
{
	  return ErrorPendSizeOS;
}

/*****************************************************************/
/*                          Stack                                */
/*****************************************************************/

int minimumStackOS(void) 
{
   int i;
   int k;
   int minimumPack= 0;
   unsigned int  *spAddr;
	 int minimumRam;

   for ( i=0; i<= TASKSIZE; i++ )  
   {
       spAddr = TaskSpPointerOS[i];

       k = 0;				
       while(  *(spAddr + 1 + k) == 0xf000000f  )
       {
            k++;
       }
				
	    DISABLE_INTERRUPT;
       PackSizeOS[i] = PaddingOS[i] - k; 
      ENABLE_INTERRUPT;				
	
	     minimumPack += PackSizeOS[i];
				
    } // for

    minimumRam = ( minimumPack + CPUREGISTER*(TASKSIZE+1) ) * WORDSIZE;
			
    return  minimumRam;
}

int autoOptimalStackOS(void) 
{
   int i;
   int k;
   int optimalPack;
   unsigned int  *spAddr;
	 int optimalRam;
	 int min = 999999;

   for ( i=0; i< TASKSIZE; i++ )  
   {
       spAddr = TaskSpPointerOS[i];

       k = 0;				
       while(  *(spAddr + 1 + k) == 0xf000000f )
       {
            k++;
       }
		
			 if ( k < min )
			 {
				   min = k;
			 }
    } 

	  optimalPack = (AutoPackItemsOS - min) * TASKSIZE + PADDINGIDLE;
		
    optimalRam = ( optimalPack + CPUREGISTER*(TASKSIZE+1) ) * WORDSIZE;
			
    return  optimalRam;
}

int autoPackItemsOS(void)
{
	  return AutoPackItemsOS;
}



int* packSizeOS(void)
{
	  minimumStackOS();
		
	  return PackSizeOS;
}

/*****************************************************************/
/*                          Semaphore                            */
/*****************************************************************/

void postSemOS(int number)
{
	  int i;     
	  int priority;
	  int *array;	
    int previousValue;	
	  int k;
	  int index;

	  if (  number >= 0 )
		{	
				 clearTableOS(PriorityOwnEventOS, CurrentPriorityOS);
			   index = currentPriorityMapEventIndexOS(SEM);
			
		     if( index >= 0 )
		     {
					  DISABLE_INTERRUPT;	
				      EventNumberTaskOS[index].numberArray = 0x0;
				      EventNumberTaskOS[index].readyNumber = -1;
            ENABLE_INTERRUPT;					 
				 }						
				 
			   if(  IsStartPendOS(SEM) )
				 {			
		        for( i = 0; i < PENDSIZE; i++  )   
            {
							 if ( EventNumberTaskOS[i].eventType == SEM )
							 {
				           priority =  EventNumberTaskOS[i].priority;	

						       if ( priority != CurrentPriorityOS )
							     {
								       array = EventNumberTaskOS[i].numberArray;
 								       previousValue = -3;
                       k = 0;   
										 
                       while( ( array[k] >= 0) && ( array[k] != previousValue ) )								 
                       { 
										      if( array[k] == number )
										   	  {
											   	  DISABLE_INTERRUPT;			
			                       EventNumberTaskOS[i].readyNumber = number;	
								            ENABLE_INTERRUPT;
												
												     resumeTaskOS(priority);	
                             break;														
											     }
										       else
											     {
										          previousValue =  array[k];
											     }
											
									         k++;
													
								       } // while
										
								    } // if ( priority != CurrentPriorityOS )
														
								 } // if ( EventNumberTaskOS[i].eventType == SEM )
	
            } // for	

				 } //	if(  IsPending(SEM) )
		
	   } // if (  number >= 0 )
		
	   if( interruptNumberOS() == 0 )
		 {		 
         executeHighestPriorityTaskOS();	 
		 }
		 else
		 {
			  DISABLE_INTERRUPT;
			   ScheduleISROS++;
			  ENABLE_INTERRUPT;	
		 } 
}


int pendSemOS(int *array, int timeout)
{
     int  index;
	   int  readyNumber= -1;
	
	   if ( interruptNumberOS() == 0 )
		 {
		    index = currentPriorityMapEventIndexOS(SEM);				 

		    if( index >= 0 )
		    {
		       DISABLE_INTERRUPT;
			 	     EventNumberTaskOS[index].priority = CurrentPriorityOS;					 
				     EventNumberTaskOS[index].numberArray = array; 
				     EventNumberTaskOS[index].eventType = SEM;					
		       ENABLE_INTERRUPT;	
	 
            pauseTaskOS(timeout);		
	
	          executeHighestPriorityTaskOS();
		    }
				else
				{
					DISABLE_INTERRUPT;
					  ErrorPendSizeOS = 1;
					ENABLE_INTERRUPT;
				}
				
			  readyNumber = pendCodeOS(array, SEM);
				
	   } // if ( interruptNo == 0 )
		 
     return readyNumber;	
}


/*****************************************************************/
/*                         Mail Message                          */
/*****************************************************************/

                 // message must be global address
void postMailOS(int number, void *messageAddr)
{
	  int i;     
	  int priority;
	  int *array;	
    int previousValue;	
	  int k;
	  int index;

	  if (  (number >= 0) && (number < MAILSIZE) )
		{	
			   ReceiveMessageOS[number] = messageAddr;	
				 clearTableOS(PriorityOwnEventOS, CurrentPriorityOS);
			   index = currentPriorityMapEventIndexOS(MAIL);	
			
		     if( index >= 0 )
		     {
					  DISABLE_INTERRUPT;
				      EventNumberTaskOS[index].numberArray = 0x0;
				      EventNumberTaskOS[index].readyNumber = -1;	
            ENABLE_INTERRUPT;					 
				 }		
				 
			   if(  IsStartPendOS(MAIL) )
				 {			
		        for( i = 0; i < PENDSIZE; i++  )   
            {
							 if ( EventNumberTaskOS[i].eventType == MAIL )
							 {
				           priority =  EventNumberTaskOS[i].priority;	

						       if ( priority != CurrentPriorityOS )
							     {
								       array = EventNumberTaskOS[i].numberArray;
 								       previousValue = -3;
                       k = 0;   
										 
                       while( ( array[k] >= 0) && ( array[k] != previousValue ) )								 
                       { 
										      if( array[k] == number )
										   	  {
											   	  DISABLE_INTERRUPT;		
			                        EventNumberTaskOS[i].readyNumber = number;	
								             ENABLE_INTERRUPT;
												
												      resumeTaskOS(priority);		
                              break;															
											     }
										       else
											     {
										           previousValue =  array[k];
											     }
											
									         k++;
													
								       } // while
										
								    } // if ( priority != CurrentPriorityOS )
									
								 } // if ( EventNumberTaskOS[i].eventType == MAIL )
	
             } // for	

				 } //	if(  IsPending(MAIL) )
		
	   } // if (  number >= 0 )
		
	   if( interruptNumberOS() == 0 )
		 {		 
         executeHighestPriorityTaskOS();	 
		 }
		 else
		 {
			  DISABLE_INTERRUPT;
			   ScheduleISROS++;
			  ENABLE_INTERRUPT;	
		 }
}


void* readMailOS(int number, char clear)
{
	  void* address = 0x0;

		if ( (number >= 0) && (number < MAILSIZE) )
		{
	     address = ReceiveMessageOS[number];
	
	     if( clear )
		   {
			   DISABLE_INTERRUPT;
			     ReceiveMessageOS[number] = 0x0;
			   ENABLE_INTERRUPT;	
		   }
	 }

		 return  address;
}


void* pendMailOS(int *array, int *readyNumberAddr, char clear, int timeout)
{
     int  index;
	   void* address= 0x0;
	
	   if ( interruptNumberOS() == 0 )
		 {
		    index = currentPriorityMapEventIndexOS(MAIL);				 

		    if( index >= 0 )
		    {
		       DISABLE_INTERRUPT;
			 	     EventNumberTaskOS[index].priority = CurrentPriorityOS;					 
				     EventNumberTaskOS[index].numberArray = array; 
				     EventNumberTaskOS[index].eventType = MAIL;					
		       ENABLE_INTERRUPT;	
	 
            pauseTaskOS(timeout);		
	
	          executeHighestPriorityTaskOS();
		    }
				else
				{
					DISABLE_INTERRUPT;
					  ErrorPendSizeOS = 1;
					ENABLE_INTERRUPT;
				}
				
		    *readyNumberAddr = pendCodeOS(array, MAIL);	
				
				if( (*readyNumberAddr >= 0) && (*readyNumberAddr < MAILSIZE) )
				{
				    address = readMailOS(*readyNumberAddr, clear);
				}
				
	   } // if 
		 
		 return address;
}


/*****************************************************************/
/*                            Flag                               */
/*****************************************************************/

void postFlagOS(int number, unsigned int modifyPublicFlag, char setOrClear )
{
	  int i;     
	  int priority;
	  unsigned int interestBits;
	  char AllOrAny;
	  char match;	
	  int *array;	
    int previousValue;	
	  int k;	
	  int index;

		if (  (number >= 0) && (number < FLAGSIZE)  )
		{
		   DISABLE_INTERRUPT;	
         switch (setOrClear)  // calculate  PublicFlagOS[]
			   {
					  case FLAG_CLEAR:  // 0
							 PublicFlagOS[number] &= ~modifyPublicFlag;	// clear the interest bits						 
					     break;
									 
					  case FLAG_SET:  // flagNumber is the index of PublicFlagOS[]
							 PublicFlagOS[number] |= modifyPublicFlag;   // set the interest bits
							 break;
				 }
		   ENABLE_INTERRUPT;	
				 
				 clearTableOS(PriorityOwnEventOS, CurrentPriorityOS);
			   index = currentPriorityMapEventIndexOS(FLAG);	
				 
		     if( index >= 0 )
		     {
		        DISABLE_INTERRUPT;
				      EventNumberTaskOS[index].numberArray = 0x0;
				      EventNumberTaskOS[index].readyNumber = -1;
				      EventNumberTaskOS[index].privateFlag = 0;		// reset private flag		
					  ENABLE_INTERRUPT;					
				 }					

			   if(  IsStartPendOS(FLAG) )
				 {				
		        for( i = 0; i < PENDSIZE; i++  )   
            {
							 if ( EventNumberTaskOS[i].eventType == FLAG )
							 {	 
								  priority = EventNumberTaskOS[i].priority;

								  if(   priority != CurrentPriorityOS   )
							    {
								     array = EventNumberTaskOS[i].numberArray;
 								     previousValue = -3;
                     k = 0;   
										
                     while( ( array[k] >= 0) && ( array[k] != previousValue ) )								 
                     {									
								         if( array[k] == number )
	         		           {
									           interestBits	= PublicFlagOS[number] & EventNumberTaskOS[i].privateFlag; // compare bit value 1

										         match = 0;
										         AllOrAny = checkSetBitOS(FlagAllOrAnyOS, priority);
											
								 		         switch(AllOrAny)
										         {
												        case FLAG_MATCH_ALL:   // 1,  public flag bits match ALL the task's interest bits( value 1 )
										  		          if ( interestBits == EventNumberTaskOS[i].privateFlag )  // every bit is match
														        {
                       		               match = 1;	
														        }
														        break;
												
														    case FLAG_MATCH_ANY:  // 0,  public flag bits match ANY ONE( value 1 ) of the task's interest bits
														        if ( interestBits != (unsigned int) 0 )  // at least one bit match (value 1)
											  		        {
                        		             match = 1;	
											 		          }													 
											  		        break;
							  		          } // switch
										
										          if ( match )
								 		          {
												       	 DISABLE_INTERRUPT;	
					           		           EventNumberTaskOS[i].readyNumber = number; 
																   EventNumberTaskOS[i].privateFlag = 0;		// reset private flag							 
													       ENABLE_INTERRUPT;		
														
                     		           resumeTaskOS(priority);	
                                   break;																	
										          }
									 
								          } // if( array[arrayIndex] == number )
										      else
											    {
										           previousValue =  array[k];
											    }
											
									        k++;
													
								      } // while												 
													
							     } // if(   priority != CurrentPriorityOS   )									 
													
							 } // if ( EventNumberTaskOS[i].eventType == FLAG )
									 
					} // for
				
		   } // if(  IsPending(FLAG) )
			   
 		 }	//  if ( number < FLAGSIZE )	

	   if( interruptNumberOS() == 0 )
		 {		 
         executeHighestPriorityTaskOS();	 
		 }
		 else
		 {
			  DISABLE_INTERRUPT;
			   ScheduleISROS++;
			  ENABLE_INTERRUPT;	
		 } 
}


int pendFlagOS(int *array, unsigned int privateFlag, char allOrAny, int timeout)
{
	   int  index;
	   int  readyNumber= -1;

	   if ( interruptNumberOS() == 0 )
		 {	
	   		 index = currentPriorityMapEventIndexOS(FLAG);				 

		    if( index >= 0 )
		    {
		         DISABLE_INTERRUPT;
			 	       EventNumberTaskOS[index].priority = CurrentPriorityOS;					 
				       EventNumberTaskOS[index].numberArray = array; 
				       EventNumberTaskOS[index].privateFlag = privateFlag;						 
				       EventNumberTaskOS[index].eventType = FLAG;					
			       ENABLE_INTERRUPT;	
					
               if ( allOrAny == FLAG_MATCH_ALL )	// 1-match All of bits    0-match ANY one bit
			         {
 			             setTableOS(FlagAllOrAnyOS, CurrentPriorityOS);  // indicate if match all of bits(1) or match any one bit(0)
			         }
	 			       else
				       {
				           clearTableOS(FlagAllOrAnyOS, CurrentPriorityOS); 
				       }	
				 
               pauseTaskOS(timeout);		
	
	             executeHighestPriorityTaskOS();
			   }
				 else
				 {
						 DISABLE_INTERRUPT;
					     ErrorPendSizeOS = 1;
						 ENABLE_INTERRUPT;	
				 }		
					
		     readyNumber = pendCodeOS(array, FLAG);						
			
	   } // if 
		 
     return readyNumber;	
}



unsigned int queryPublicFlagOS(int flagNumber)
{
	    return PublicFlagOS[flagNumber];
}


                         // return value is either 1 or 0
char checkPublicFlagBitOS(int flagNumber, char bitNumber)
{
	   return  checkSetBitOS( &PublicFlagOS[flagNumber], bitNumber);
}


/*****************************************************************/
/*                           Mutex                               */
/*****************************************************************/

void postMutexOS(void)
{
	  int i;
	  int number;
	  int index;
	  int priority;
	  int minPriority= 99999999;
	  int minIndex= -1;
	  int *array;	
	
		if( interruptNumberOS() == 0 )  // ISR can not call postMutexOS()
		{		
	     index = currentPriorityMapEventIndexOS(MUTEX);	
	     if( index >= 0 )
	     {
		   	  number = EventNumberTaskOS[index].readyNumber;  // own the mutex number
			   	if( (number >= 0) && (number < MUTEXSIZE) && (MutexOwnerOS[number] == CurrentPriorityOS) )
			   	{
					   clearTableOS(PriorityOwnEventOS, CurrentPriorityOS);
					  DISABLE_INTERRUPT;	
				     EventNumberTaskOS[index].numberArray = 0x0;
				     EventNumberTaskOS[index].readyNumber = -1;
						 MutexOwnerOS[number] = -1;		 // mutex is free	
					  ENABLE_INTERRUPT;		
			
						 if( IsStartPendOS(MUTEX) )
						 {
		            for( i = 0; i < PENDSIZE; i++  )   // ready the highest priority
                {
							     if ( EventNumberTaskOS[i].eventType == MUTEX )
							     {
				               priority =  EventNumberTaskOS[i].priority;	

						           if ( priority != CurrentPriorityOS )
							         {
							             array = EventNumberTaskOS[i].numberArray;   
 
									         if(  (array[0] == number) && (priority < minPriority) )
									         {
												      minPriority = priority;
												      minIndex = i;													
								           }
							         } // if ( priority != CurrentPriorityOS )
														
							     } // if ( EventNumberTaskOS[i].eventType == SEM )
	
                } // for	
					   } // if( IsStartPendOS(MUTEX) )
						
						 if( minIndex >= 0 )
						 {
							  DISABLE_INTERRUPT;	
							   MutexOwnerOS[number] = minPriority;						
			           EventNumberTaskOS[minIndex].readyNumber = number;	
							 	 EventNumberTaskOS[minIndex].numberArray = 0x0;	
							  ENABLE_INTERRUPT;
												
							   resumeTaskOS(minPriority);							
						 }					
						
	        } // if(  (number >= 0) && (number < MUTEXSIZE)  )
				
	     } // if( index >= 0 )					 
	
		   executeHighestPriorityTaskOS();	  
		
		} // if( interruptNumberOS() == 0 )
		
}


int pendMutexOS(int *array, int timeout)
{
     int  index;
	   int  number;
	   int  readyNumber= -1;

	   if ( interruptNumberOS() == 0 )
		 {
	      index = currentPriorityMapEventIndexOS(MUTEX);				 

		    if( index >= 0 )
	      {
					   number = array[0];  // only one mutex
					   if( MutexOwnerOS[number] == CurrentPriorityOS )
						 {
							    return number;
						 }
						 
		       DISABLE_INTERRUPT;
			 	     EventNumberTaskOS[index].priority = CurrentPriorityOS;					 
				     EventNumberTaskOS[index].numberArray = array; 
				     EventNumberTaskOS[index].eventType = MUTEX;					
		       ENABLE_INTERRUPT;	
			 
			       if( (number < MUTEXSIZE) && (MutexOwnerOS[number] < 0) )
				     {
							   setTableOS(PriorityOwnEventOS, CurrentPriorityOS);
		           DISABLE_INTERRUPT;											
                 MutexOwnerOS[number] = CurrentPriorityOS;
							   EventNumberTaskOS[index].numberArray = 0x0;		
			           EventNumberTaskOS[index].readyNumber = number;
		           ENABLE_INTERRUPT;												
			        } // if( number < MUTEXSIZE )
	            else
			   	    {		
                 pauseTaskOS(timeout);		
	
	               executeHighestPriorityTaskOS();
			   	    }
				 
		     }	// if( index >= 0 )
				 else
				 {
					 DISABLE_INTERRUPT;
					   ErrorPendSizeOS = 1;
					 ENABLE_INTERRUPT;
				 }
				
				 readyNumber = readReadyNumberOS(MUTEX);
		 
		 } // if ( interruptNumberOS() == 0 )
		 
     return readyNumber;		// the only one (array) element
}

/*****************************************************************/
/*                            Queue                              */
/*****************************************************************/


int postQOS(int number, void *messageAddr)
{
	  int i;     
	  int priority;
	  int inIndex;
	  int *array;	
	  int k;	
    int previousValue;
	  int remainItems;

		if (  (number >= 0) && (number < QSIZE) && ( QBodyOS[number].q != 0x0) )
		{	
				 clearTableOS(PriorityOwnEventOS, CurrentPriorityOS);	
	     DISABLE_INTERRUPT;
		   	 inIndex = QBodyOS[number].inIndex++;		
			   QBodyOS[number].q[inIndex] = messageAddr; // user must handle the data type.	
			   QBodyOS[number].items++;

				 if(  QBodyOS[number].items > QLENGTH )
			   { 
              QOverflowOS[number] = 1;
					    QBodyOS[number].items = QLENGTH;
			   }	

				 if(  QBodyOS[number].inIndex >= QLENGTH )
			   {
				      QBodyOS[number].inIndex = 0;				
			   }
		   ENABLE_INTERRUPT;
				 
				 remainItems = QLENGTH - QBodyOS[number].items;
				 
				 if(  IsStartPendOS(QUEUE) )
				 { 
		        for( i = 0; i < PENDSIZE; i++  )   
            {
							 if ( EventNumberTaskOS[i].eventType == QUEUE )
							 {
				          priority = EventNumberTaskOS[i].priority;	
								 
						      if ( priority != CurrentPriorityOS )
					        {
							       array = EventNumberTaskOS[i].numberArray;
	 							     previousValue = -3;
                     k = 0; 

                     while( ( array[k] >= 0) && ( array[k] != previousValue ) )								 
                     {
 									       if( array[k] == number )
							           {
							                resumeTaskOS(priority);	
                              break;														 
						  	          } 
											    else
											    {
										           previousValue =  array[k];
											    }	

										      k++;	
													
							        } // while
							
						       } // if ( priority != CurrentPriorityOS )
								
							  } // if ( EventNumberTaskOS[i].eventType == Q )

             } // for	
				 
				  }	// if(  IsPending(Q) ) 

	   } // if ( number >= 0 )

	   if( interruptNumberOS() == 0 )
		 {		 
         executeHighestPriorityTaskOS();	 
		 }
		 else
		 {
			  DISABLE_INTERRUPT;
			   ScheduleISROS++;
			  ENABLE_INTERRUPT;	
		 }
		 
		 return remainItems;
}



void* readQOS(int number)
{
	  int   i;
	  int   outIndex;
	  char  setBit;
	  void* retrieveAddress = 0x0;
	
		 setBit = checkSetBitOS(PriorityOwnEventOS, CurrentPriorityOS);
	
     if((setBit > 0) && (number >= 0) && (number < QSIZE) && (QBodyOS[number].items > 0))
		 {
			  DISABLE_INTERRUPT;	
	       ItemsPendQOS[number] = QBodyOS[number].items;	
				ENABLE_INTERRUPT;	
			 
		     for(i=0; i<QBodyOS[number].items; i++)
		     {
					  DISABLE_INTERRUPT;	
	            outIndex = QBodyOS[number].outIndex++;
              PendQRetrieveOS[number][i] = QBodyOS[number].q[outIndex];	
              retrieveAddress = PendQRetrieveOS[number];					 
					 
		          if( QBodyOS[number].outIndex >= QLENGTH )
			        {
					        QBodyOS[number].outIndex = 0;
			        }
						 ENABLE_INTERRUPT;						
		      } // for	
				
				 DISABLE_INTERRUPT;	
				  QBodyOS[number].items = 0;   // clear message
				  QOverflowOS[number] =0;
				 ENABLE_INTERRUPT;		
					
		 } // if(  (setBit > 0)	&& (number >= 0)  )
		 
	   return retrieveAddress;
}


int findItemNumberOS(int *array)
{
	   int  k;	
     int  previousValue;		
     int  number;	
	
 		 previousValue = -3;
     k = 0;   
		 number = array[0];
	
     while( ( number >= 0) && (number < QSIZE) && ( number != previousValue ) && (QBodyOS[number].items == 0) )								 
     { 
          previousValue = number;
          k++;
			    if ( (array[k] >= 0) && (array[k] < QSIZE) )
					{
		           number = array[k];	
					}										
		 }	
	             // number == previousValue
		 if ( (number >= 0) && (number < QSIZE) )
		 {
		    if ( QBodyOS[number].items == 0 )
		    {
			      number = -1;   // no item
		    }
	   }
		 else
		 {
			    number = -1;
		 }
		 
		 return number;
}


void* pendQOS(int *array, char readQ, int timeout)
{
     int   index;		
     int   number;
	   char  pend;
	   void* retrieve = 0x0;

	   if ( interruptNumberOS() == 0 )
		 {	
	   	  index = currentPriorityMapEventIndexOS(QUEUE);				 

		    if( index >= 0 )
		    { 
		       DISABLE_INTERRUPT;
			 	     EventNumberTaskOS[index].priority = CurrentPriorityOS;					 
				     EventNumberTaskOS[index].numberArray = array; 
				     EventNumberTaskOS[index].eventType = QUEUE;					
		       ENABLE_INTERRUPT;	

					   number = findItemNumberOS(array);
					                
			       if( (number >= 0) && (number < QSIZE) )  // find items
				     { 										
							    setTableOS(PriorityOwnEventOS, CurrentPriorityOS);
                  pend = 0;			
			        } 			
              else
			   	    { 
									pend = 1;  
                  pauseTaskOS(timeout);		
	                executeHighestPriorityTaskOS();
								                 // wake up from pending
					        number = findItemNumberOS(array);										
			   	    } // else
							
							if( number >= 0 )  // find items
							{ 
					      DISABLE_INTERRUPT;
							
							    SleepPendQOS[number] = pend;         
		
							    OverflowPendQOS[number] = QOverflowOS[number]; 	
							
							    ItemsPendQOS[number] = 0;    // do not read
							    if( readQ )
							    { 
										  readQOS(number);         
				              retrieve = PendQRetrieveOS[number];										
							    }

						    ENABLE_INTERRUPT;
									
						  } // if( number >= 0 )
							
		     } // if( index >= 0 )
				 else
				 { 
					 DISABLE_INTERRUPT;
					   ErrorPendSizeOS = 1;
					 ENABLE_INTERRUPT;
				 }

	   } // if	 

     return retrieve; 	 // the first element in (number) array  that was posted
}


int getQNumberOS(void* retrieveAddress)
{
	  int i;
	  int number= -1;
	
	  for(i=0; i<QSIZE; i++)
		{
			  if( (int)retrieveAddress == (int)&PendQRetrieveOS[i][0] )
				{
					   number = i;
				}
		}
		
		return number;
}


char qSleepOS(void* retrieveAddress)
{
	  int  number;
	  char sleep= 0;
	
	  number = getQNumberOS(retrieveAddress);
	
	  if ( (number >= 0) && (number < QSIZE) )
		{
			  sleep = SleepPendQOS[number];
			  SleepPendQOS[number] = 0;
		}
		
		return sleep;
}

char qOverflowOS(void* retrieveAddress)
{
	  int  number;
	  char overflow= 0;
	
	  number = getQNumberOS(retrieveAddress);
	
	  if ( (number >= 0) && (number < QSIZE) )
		{
			  overflow = OverflowPendQOS[number];
			  OverflowPendQOS[number] = 0;
		}
		
		return overflow;
}


int qItemsOS(void* retrieveAddress)
{
	  int number;
	  int items= 0;
	
	  number = getQNumberOS(retrieveAddress);
	
	  if ( (number >= 0) && (number < QSIZE) )
		{
			  items = ItemsPendQOS[number];
			  ItemsPendQOS[number] = 0;;
		}
		
		return items;
}


/*****************************************************************/
/*                           Delay                               */
/*****************************************************************/

void delayTickOS(int tick)
{
		if( interruptNumberOS() == 0 )
		{	
       if ( tick > 0 )
       {
           pauseTaskOS(tick);		 
       }
		
       executeHighestPriorityTaskOS();	
		}

} 



void delayTimeOS( int hour, int minute, int second, int  mS)
{
    int tick;
	  int remainder;	

		if( interruptNumberOS() == 0 )
		{	
	     if ( ( hour >= 0 ) && (  minute >= 0 ) && ( second >= 0 ) && ( mS >= 0 ) )
		   {
	         tick      = TickPerSecondOS * ( hour * 3600 + minute * 60 + second );
	         tick     += TickPerSecondOS * mS / 1000; 
			 	   remainder = TickPerSecondOS * mS % 1000; 
			 
			     if ( remainder > 500 )
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


void delayUntilEqualOS(int *a, int *b)
{
	 int  k= 0;
	 char find= 0;
	
	 if( interruptNumberOS() == 0 )
	 {
       if ( (a != 0x0) && (b != 0x0) )
       {
       	    while( (k<UNTILSIZE) && (!find) )
       	    {
       	   	    if( (DelayUntilPriorityOS[k] == CurrentPriorityOS) || (DelayUntilPriorityOS[k] == -1) )
       	   	    {
       	   	       DISABLE_INTERRUPT;
       	   	        DelayUntilOS[k][0] = a;
        	          DelayUntilOS[k][1] = b;      	   
          	        DelayUntilPriorityOS[k] = CurrentPriorityOS; 
				           ENABLE_INTERRUPT; 
				  
					          find = 1;
					          break;  	
       	   	    }
       	   	    k++;
			      }
	     	
            pauseTaskOS(TIMEOUT_INFINITE);		 
       }

       executeHighestPriorityTaskOS();	
	 }
}


void delayUntilTrueOS(int *a)
{
	 int  k= 0;
	 char find= 0;
	
	 if( interruptNumberOS() == 0 )
	 {
       if ( a != 0x0 )
       {
       	    while( (k<UNTILSIZE) && (!find) )
       	    {
       	   	    if( (DelayUntilPriorityOS[k] == CurrentPriorityOS) || (DelayUntilPriorityOS[k] == -1) )
       	   	    {
       	   	       DISABLE_INTERRUPT;
       	   	        DelayUntilOS[k][0] = a;
        	          DelayUntilOS[k][1] = 0x0;      	   
          	        DelayUntilPriorityOS[k] = CurrentPriorityOS; 
				           ENABLE_INTERRUPT; 
				  
					          find = 1;
					          break;  	
       	   	    }
       	   	    k++;
			      }
	     	
            pauseTaskOS(TIMEOUT_INFINITE);		 
       }

       executeHighestPriorityTaskOS();	
	 }
}


char checkDelayUntilOS(void)
{
	 int  k;
	 char resume= 0;
	
	 for(k=0; k<UNTILSIZE; k++)	
	 {                                 // until true
		 	 if(  (*DelayUntilOS[k][0] != 0) && (DelayUntilOS[k][1] == 0x0) &&(DelayUntilPriorityOS[k] >= 0)  )
	     {	
	    	   resumeTaskOS( DelayUntilPriorityOS[k] );
			     resume = 1;
		   } 
			                              // until equal
	     else if(  (*DelayUntilOS[k][0] == *DelayUntilOS[k][1]) && (DelayUntilPriorityOS[k] >= 0)  )
	     {	
	    	   resumeTaskOS( DelayUntilPriorityOS[k] );
			     resume = 1;
		   } 
	 } 
	 
	 return resume;
}


/*****************************************************************************/
/*                             Memory Management                             */
/*****************************************************************************/



long double* memoryAddressOS(int bulkNo)
{
    long double *bulkAddress;		
	
		 bulkAddress = (long double *)((unsigned int)PoolOS + (unsigned int)(bulkNo * BULKBYTES) );

	   return bulkAddress;
}



void memoryOS(int bytes, int getNo)
{
	  int  r;           // bulk number in available memory
 	  int  margeNo;     // marge bulk number
	  int  length1;     // free bulk length + 1(marge bulk)
	  int  desiredBulkLength;
	  int  frontBulkNo;
	  int  rearBulkNo;
	  int  minimum= 999999999;
	  char found;	
	  char setBit;

	  MallocAddressOS = 0x0;

	  if ( getNo >= 0 )
		{
	     RelyMargeDangerOS[getNo][0] = 0x0;
		}

		if( (bytes > 0) && (GLOBALBULKLENGTH>1) && (getNo < GETMEMORYSIZE) && ((getNo >= 0)||(getNo==(int)MALLOC)) )
		{
		   desiredBulkLength = bytes%BULKBYTES ? bytes/BULKBYTES+1 : bytes/BULKBYTES ;	
			
			 if( desiredBulkLength == 0 )
			 {
				   desiredBulkLength = 1;
			 }
			 
	     found = 0;	
       margeNo = 0;
	     while( (margeNo < GLOBALBULKLENGTH-desiredBulkLength) && (!found) )
		   {
			     setBit = checkSetBitOS(FreeBulkNoOS, margeNo); 
			     if (  setBit )  // memory available
				   { 
						   length1 = 1;
					     setBit = checkSetBitOS(FreeBulkNoOS, margeNo+1); // find continuous free bulk
							 
	             while( (margeNo+length1 < GLOBALBULKLENGTH) && setBit   )
		           {
			             length1++;
								   if( margeNo+length1 < GLOBALBULKLENGTH )
									 {
					            setBit = checkSetBitOS(FreeBulkNoOS, margeNo+length1);	// check continuous free bulk	
                   }										
		           }                        //using   not use   using    
                                       // 0 0 0 1 1 1 1 1 0 0  FreeBulkNoOS[bufferNo]=1
                                       //       |     |   |         
						                           //       |     |   margeNo+length1    length1=5, bulkLength=3
                                      //  bufferNo rearBulkNo

					     frontBulkNo = margeNo<1 ? 0 : margeNo+1;  // if margeNo=0, margeNo is data bulk
				
					 	   if ( margeNo+length1 == GLOBALBULKLENGTH )
					 	   {
							     if ( (margeNo==0) && (length1-1 >= desiredBulkLength) && (length1 < minimum)  )
						       {
								      rearBulkNo = desiredBulkLength-1;
						          found= 1;		
                      minimum = length1;									 
						       }								 
							     else if ( (length1-2 >= desiredBulkLength) && (length1 < minimum)  )
						       {
								      rearBulkNo = margeNo + desiredBulkLength;
						          found= 1;		
                      minimum = length1;									 
						       }	
				           else
				           {
					             margeNo++;
				           }									 
						   }
						   else if ( !setBit )   //FreeBulkNoOS[margeNo+length1]=0 , FreeBulkNoOS[margeNo+length1-1]=1
						   {
                  if (  (length1-2 >= desiredBulkLength) && (length1 < minimum)  )
								  {
									    rearBulkNo = margeNo<1 ? desiredBulkLength-1 : margeNo+desiredBulkLength;
						          found= 1;	
                      minimum = length1;															 
								  }	
									else if( (margeNo == 0) && (length1 > desiredBulkLength) && (length1 < minimum))
									{
									    rearBulkNo = desiredBulkLength-1;
						          found= 1;	
                      minimum = length1;										
									}
						      else    // memory is too small
						      {
							       margeNo += length1;
						      }								
						   }
						 
				  }  //  if (  setBit )
				  else
				  {
					    margeNo++;
				  }
				
		   } // while
	
	     if( found )
		   {
				 	for(r= frontBulkNo; r<= rearBulkNo; r++ ) //  FreeBulkNoOS[margeNo]=1
				  {
			       clearTableOS(FreeBulkNoOS, r);			// using
				  }
				
	 	    DISABLE_INTERRUPT;						
				  if( getNo >= 0 )
					{
				     RelyMargeDangerOS[getNo][0] = memoryAddressOS(frontBulkNo);   // relyAddress	
			       StartBulkNoOS[getNo] = frontBulkNo;
	 					 
					   if ( rearBulkNo <= GLOBALBULKLENGTH-2 )
				     {
					      RelyMargeDangerOS[getNo][1] = memoryAddressOS(rearBulkNo+1);		// margeAddress			
				     }
				
				     if ( rearBulkNo <= GLOBALBULKLENGTH-3 )
				     {
					      RelyMargeDangerOS[getNo][2] = (void*)( (unsigned int) RelyMargeDangerOS[getNo][1] + (unsigned int)BULKBYTES );		// dangerAddress	
				     }

					   if( (margeNo+length1-1) > (rearBulkNo+2) )
					   {
			          DangerBytesOS[getNo] = ( (margeNo+length1-1) - (rearBulkNo+2) ) * BULKBYTES;	
					   }
						 else if( (margeNo == 0) && (desiredBulkLength < length1-1) )
					   {
						    DangerBytesOS[getNo] = (length1 - 2 - desiredBulkLength) * BULKBYTES;
					   }
					   else if( (margeNo == 0) && (GLOBALBULKLENGTH-1 > rearBulkNo+1) && (length1-1 > GLOBALBULKLENGTH-4) )
					   {  
			          DangerBytesOS[getNo] = (GLOBALBULKLENGTH - 3 - rearBulkNo) * BULKBYTES;						 
					   }
					 
				  }// if( getNo >= 0 )
				  else if( getNo == (int)MALLOC )
				  {
					   MallocAddressOS = memoryAddressOS(frontBulkNo);
				  }
				 ENABLE_INTERRUPT; 
		   } // if( found )
			 else
			 {
				 DISABLE_INTERRUPT;	
				   LackNoOS[getNo] = 1;
				   LackNoAllOS[getNo]++;
				 ENABLE_INTERRUPT;
			 }
			 
	  } // if( (bytes > 0) &&

}


void* getMemoryOS(int bytes, int getNo)
{
		 if( (getNo >= 0) && (getNo < GETMEMORYSIZE) )
		 {
	      if( StartBulkNoOS[getNo] < 0 )
		    {		
            memoryOS(bytes, getNo);
	      } 
		    else   // memory leak
		    {
					DISABLE_INTERRUPT;
					 RelyMargeDangerOS[getNo][0] = 0x0;
           LeakNoOS[getNo] = 1;
					 LeakNoAllOS[getNo]++;
          ENABLE_INTERRUPT;					
		    }
	   }

		 return  RelyMargeDangerOS[getNo][0];
}


void* mallocOS(int bytes)
{		
	  memoryOS(bytes, MALLOC);

		return  MallocAddressOS;
}



char putMemoryOS(int getNo)
{
	  int   k;
	  int   frontBulkNo;
	  char  error= 1;
	  char  setBit;	

	  if( (getNo >= 0) && (getNo < GETMEMORYSIZE) )
		{
			  frontBulkNo = StartBulkNoOS[getNo];

        if( (frontBulkNo >= 0) && (frontBulkNo < GLOBALBULKLENGTH) )
			  {
			      if( !checkSetBitOS(FreeBulkNoOS, frontBulkNo) )
				    {
			          for( k = frontBulkNo; k<GLOBALBULKLENGTH; k++)
			          {
					          setBit = checkSetBitOS(FreeBulkNoOS, k);
			 	            if ( setBit  ) // k is margeBulkNo
					          {
									     error = 0;  // StartBulkNoOS[getNo] = -1
									    DISABLE_INTERRUPT;
					             StartBulkNoOS[getNo] = -1;
						           DangerBytesOS[getNo] = 0;
						           RelyMargeDangerOS[getNo][0] = (void*)0x0;
						           RelyMargeDangerOS[getNo][1] = (void*)0x0;
						           RelyMargeDangerOS[getNo][2] = (void*)0x0;						
						          ENABLE_INTERRUPT;								
           	           break;									
					          }
											
			  	          setTableOS(FreeBulkNoOS, k); 			
			          }
				
			      } // if( !checkSetBitOS(
		
				} // if( (bulkNo >= 0) &&
	  } 

    return error;	
}


void freeOS(void* ptr)
{
	  int   k;
	  int   frontBulkNo= -1;
	  char  setBit;	

	  for( k=0; k<GLOBALBULKLENGTH; k++)
	  {
				if( (int)ptr == (int)memoryAddressOS(k) )
				{
            frontBulkNo = k;
					  break;
				}									
		}								

    if( frontBulkNo >= 0 )
		{		
			  if( !checkSetBitOS(FreeBulkNoOS, frontBulkNo) )
				{
			      for( k= frontBulkNo; k<GLOBALBULKLENGTH; k++)
			      {
					      setBit = checkSetBitOS(FreeBulkNoOS, k);
			 	        if ( setBit  ) // k is margeBulkNo
					      {
           	           break;									
					      }
							
			  	       setTableOS(FreeBulkNoOS, k); 			
			      }
				
			  } // if( !checkSetBitOS(
		}						
}


void* getMemoryWithPutOS(int bytes, int getNo)
{		
	   putMemoryOS(getNo);	

	   getMemoryOS(bytes, getNo);

		 return  RelyMargeDangerOS[getNo][0];
}



int queryFreeBulkNoOS(char* result)
{
	 int i;
	
	 for (i=0; i<GLOBALBULKLENGTH; i++)
	 {
		   result[i] = checkSetBitOS(FreeBulkNoOS, i);
	 }
	 
	 return GLOBALBULKLENGTH;
}



int getMemoryNoOS(void* relyAddress)
{
	  int i;
    int frontBulkNo;		
	  int getNo= -1;
	
		frontBulkNo = ( (int)relyAddress - (int)PoolOS ) / (int)BULKBYTES;

	  for(i=0; i<GETMEMORYSIZE; i++)
		{
       if ( frontBulkNo == StartBulkNoOS[i] )
			 {
				   getNo = i;
			 }
		}			
	
	   return getNo;
}


void* margeAddressOS(void* relyAddress)
{		
	  int   getNo;
	  void* margeAddress = 0x0;
	
	  getNo = getMemoryNoOS(relyAddress);
	
	  if( getNo >= 0 )
		{
			  margeAddress = RelyMargeDangerOS[getNo][1];
		}
	
		return  margeAddress;
}


void* dangerAddressOS(void* relyAddress, int *dangerBytes)
{		
	  int   getNo;
	  void* dangerAddress = 0x0;
	
	  getNo = getMemoryNoOS(relyAddress);
	  *dangerBytes = 0;
	
	  if( getNo >= 0 )
		{
				dangerAddress = RelyMargeDangerOS[getNo][2];
	      *dangerBytes = DangerBytesOS[getNo];
		}

		return  dangerAddress;
}



int checkFreeMemoryOS(char maxMin)
{
		int  i;
		int  continueBulk = 0;
		int  minimum = 999999999;
	  int  maximum = -1;
	  int  continueByte;
		char stop;
    char result[GLOBALBULKLENGTH+1];	// GLOBALBULKLENGTH may be 0

	
	  queryFreeBulkNoOS(result);	
		
    for(i=1;  i<GLOBALBULKLENGTH-1; i++ )
		{
		    if( (result[i-1] == 1) && (result[i] == 1) )
		    { 
				     continueBulk++;
				     stop = 0;
					   if ( continueBulk == GLOBALBULKLENGTH-2 )
						 {
							   continueBulk++;
						 }
		    }
		    else
			  {
				     stop = 1;
			  }
				
			  if( stop || i >= GLOBALBULKLENGTH-2 )
			  {
			       if( (continueBulk > maximum) && (continueBulk > 0) )
			       {
					       maximum = continueBulk;
			       }
						
			       if( (continueBulk < minimum) && (continueBulk > 0) )
			       {
					       minimum = continueBulk;
			       }
							 
						 continueBulk = 0;
			   } // if( stop )			
				
		 }	// for	

     continueByte = maxMin ? BULKBYTES*maximum : BULKBYTES*minimum ;

		 if( (minimum > GLOBALBULKLENGTH) && (maximum < 0) )         
     {
          continueByte = 0;
     }

	   return  continueByte;
}



int maxFreeMemoryOS(void)
{
	  return  checkFreeMemoryOS(1);
}



int minFreeMemoryOS(void)
{
	  return  checkFreeMemoryOS(0);
}


int leakMemoryNoOS(void)
{
	  int i;
	  int leakNo = -1;    // OK
	
	  for (i=0; i<GETMEMORYSIZE; i++)
	  {
			  if ( LeakNoOS[i] > 0 )
				{
					  leakNo = i;
					  LeakNoOS[i] = 0;
					  break;
				}
		}
	  return leakNo;
}


int lackMemoryNoOS(void)
{
	  int i;
	  int lackNo = -1;    // OK
	
	  for (i=0; i<GETMEMORYSIZE; i++)
	  {
			  if ( LackNoOS[i] > 0 )
				{
					  lackNo = i;
					  LackNoOS[i] = 0;
					  break;
				}
		}
	  return lackNo;
}


int* leakMemoryNoAllOS(void)
{
	  return LeakNoAllOS;
}


int* lackMemoryNoAllOS(void)
{
	  return LackNoAllOS;
}


/*****************************************************************/
/*                        Task Load                              */
/*****************************************************************/

int* relativeTaskLoadOS(void)
{
    int i;	
	  int third;
    long double   totalClock = 0.0;

	  for (i=0; i< TASKSIZE; i++)  // exclude idleTask()
		{
			  totalClock += (long double)TaskExecuteClockOS[i];
		}
		
	  for (i=0; i< TASKSIZE; i++)
		{
			  TaskLoadOS[i] = (int)((long double)TaskExecuteClockOS[i] / totalClock * (long double)100);   // int number
			
			  third = (int)((long double)TaskExecuteClockOS[i] / totalClock * (long double)1000) % 10;
			  if ( third >= 5)
				{
					  TaskLoadOS[i]++;
				}
		}	

		return  TaskLoadOS;	
}


int idleTaskLoadOS(void)
{
    int i;	
		int third;
	  int percentage;
    long double   totalClock = 0.0;

	  for (i=0; i<= TASKSIZE; i++)  // include idleTask()
		{
			  totalClock +=  (long double)TaskExecuteClockOS[i];
		}
		
		percentage = (int)((long double)TaskExecuteClockOS[TASKSIZE] / totalClock * 100); 
		
		third = (int)((long double)TaskExecuteClockOS[TASKSIZE] / totalClock * (long double)1000) % 10;
		if ( third >= 5)
		{
				  percentage++;
		}
				
		return percentage;
	}

/*************************************************************/



