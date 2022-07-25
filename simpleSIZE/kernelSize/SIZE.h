
#include "PORT.h"

#if   defined ( ARM )     
  #define  WORDSIZE    4
       // core register address
  #define SystickControlRegisterOS       ( *( ( volatile unsigned int* ) 0xE000E010 ) )
  #define SystickLoadRegisterOS          ( *( ( volatile unsigned int* ) 0xE000E014 ) )
  #define SystickCurrentValueRegisterOS  ( *( ( volatile unsigned int* ) 0xE000E018 ) )
  #define InterruptControlRegisterOS     ( *( ( volatile unsigned int* ) 0xE000ED04 ) )
  #define HandlerPriorityRegisterOS      ( *( ( volatile unsigned int* ) 0xE000ED20 ) )  //SHPR3
#endif

#if   defined ( CM0 )
	#define  CPUREGISTER         17
#endif
	
	  // task
#define  TASKSIZE          8 
#define  STACKRAMBYTES     (CPUREGISTER + 40)*WORDSIZE*(TASKSIZE + 1)

    // event and service
#define  PENDSIZE           TASKSIZE
#define  GETMEMORYSIZE      2*TASKSIZE
#define  QSIZE              TASKSIZE    
#define  QBYTES             32	
#define  MAILSIZE           TASKSIZE
#define  FLAGSIZE           TASKSIZE	
#define  MUTEXSIZE          TASKSIZE
#define  UNTILSIZE          TASKSIZE
#define  MEMORYPOOLBYTES    TASKSIZE*(QBYTES + 96)

    // padding part of task stack(selective)
int PaddingOS[TASKSIZE+1] = {0};  
#define  PADDINGIDLE       0      // idleTaskOS() padding

