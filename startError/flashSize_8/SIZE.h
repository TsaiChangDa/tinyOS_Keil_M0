
#include "PORT.h"

	  // task
#define  TASKSIZE           1 
#define  STACKRAMBYTES   1000

    // event and service
#define  PENDSIZE           5
#define  MEMORYPOOLBYTES   48
#define  GETMEMORYSIZE      2
#define  QSIZE              1    
#define  QBYTES             8	
#define  MAILSIZE           1
#define  FLAGSIZE           1	
#define  MUTEXSIZE          1
#define  UNTILSIZE          2

    // padding part of task stack(selective)
int PaddingOS[TASKSIZE+1] = {0}; 
#define  PADDINGIDLE       0      // idleTaskOS() padding

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
	



