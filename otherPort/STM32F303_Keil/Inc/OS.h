
//#include "PORT.h"

//#if   defined ( CM0 )
#ifndef __OS_H
#define __OS_H

#include <stdint.h>
#include "system_stm32f3xx.h"

  // #define  ASSERTOS(x) if( !(x) ){ __asm ( "cpsid i		\n" ); for(;;); }
	// #define  DISABLE_INTERRUPT  __asm( "cpsid i		\n" );
  // #define  ENABLE_INTERRUPT   __asm( "cpsie i		\n" );
   #define  ASSERTOS(x) if( !(x) ){ __asm volatile ( "cpsid i		\n" ); for(;;); }
   #define  DISABLE_INTERRUPT  __asm volatile ( "cpsid i		\n" );
  #define  ENABLE_INTERRUPT   __asm volatile ( "cpsie i		\n" );

   //#if   defined ( NXP )
   //   #include <LPC11xx.h>
	    #define  CPUClock   SystemCoreClock
   //#endif
//#endif

#define  OSCLOCK_1S       CPUClock 
#define  OSCLOCK_500mS    CPUClock/2
#define  OSCLOCK_200mS    CPUClock/5
#define  OSCLOCK_100mS    CPUClock/10     // 10 tick per second
#define  OSCLOCK_10mS     CPUClock/100
#define  OSCLOCK_1mS      CPUClock/1000
#define  OSCLOCK_100uS    CPUClock/10000
#define  OSCLOCK_10K      10000
#define  OSCLOCK_100K     100000
#define  OSCLOCK_1M       1000000

          // select OS tick
#define  clockOS           OSCLOCK_100mS 

#define  TIMEOUT_INFINITE  -1

#define  SEM               0
#define  MAIL              1	
#define  FLAG              2
#define  QUEUE             3
#define  MUTEX             4

            // used in postFlagOS()
#define  FLAG_CLEAR        0
#define  FLAG_SET          1
	          // used in pendFlagOS()
#define  FLAG_MATCH_ANY    0
#define  FLAG_MATCH_ALL    1

            // used in memoryOS()
#define  MALLOC           -2

/*****************************************************************/
/*                    API                                        */
/*****************************************************************/
       // Kernel
typedef void (*fn_ptr_t)();
//char         startOS(void (*[])(void), int, int, int);
char         startOS(fn_ptr_t * , int, int, int);

void         queryReadyTableOS(char*);
void         endISRpreemptOS(void);
int          nonBlockingCallbackOS(int (*)(void));
char         errorPendSizeOS(void);
       // Stack
int          ramToPaddingOS(int, int);
int          paddingToRamOS(int, int);						
int          autoPackItemsOS(void);
int          minimumStackOS(void);
int          autoOptimalStackOS(void);
int*         packSizeOS(void);
       // Delay
void         delayTickOS(int);
void         delayTimeOS(int, int, int, int);
void         delayUntilEqualOS(int*, int*);
void         delayUntilTrueOS(int*);
      // Semaphore
void         postSemOS(int);
int          pendSemOS(int*, int);
      // Mail Message
void         postMailOS(int, void*);
void*        readMailOS(int, char);
void*        pendMailOS(int*, int*, char, int);
      // Flag
void         postFlagOS(int, unsigned int, char);
int          pendFlagOS(int*, unsigned int, char, int);
char         checkPublicFlagBitOS(int, char);
unsigned int queryPublicFlagOS(int);
      // Mutex
void          postMutexOS(void);
int           pendMutexOS(int*, int);
      // Queue
int           postQOS(int, void*);
void*         readQOS(int);
void*         pendQOS(int*, char, int);
char          qSleepOS(void*);
char          qOverflowOS(void*);
int           qItemsOS(void*);
int           getQNumberOS(void*);
      // Memory Management
void*         getMemoryOS(int, int);
void*         getMemoryWithPutOS(int, int);
void*         margeAddressOS(void*);
void*         dangerAddressOS(void*, int*);
char          putMemoryOS(int); 
void*         mallocOS(int);
void          freeOS(void*);
int           maxFreeMemoryOS(void);
int           minFreeMemoryOS(void);
int           queryFreeBulkNoOS(char*);
int           leakMemoryNoOS(void);
int           lackMemoryNoOS(void);
int*          leakMemoryNoAllOS(void);
int*          lackMemoryNoAllOS(void);
      // Task Loading
int*          relativeTaskLoadOS(void);
int           idleTaskLoadOS(void);

#endif

