
#ifndef _SIZE_H
#define _SIZE_H

#include "PORT.h"

	  // task
#define  TASKSIZE          5
//#define  STACKRAMBYTES     2500
#define  STACKRAMBYTES     0x460
    // event and service
#define  PENDSIZE           0
#define  MEMORYPOOLBYTES    0
#define  GETMEMORYSIZE      0
#define  QSIZE              0    
#define  QBYTES             0	
#define  MAILSIZE           0
#define  FLAGSIZE           0
#define  MUTEXSIZE          0
#define  UNTILSIZE          0

    // padding part of task stack(selective)
//int PaddingOS[TASKSIZE+1] = {0};  
int PaddingOS[TASKSIZE+1] = {0x49, 0x29, 0x19, 0x11, 0x16}; 
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
	
#endif


