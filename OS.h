
#include "PORT.h"

#if   defined ( CM0 )
	 #define  ASSERTOS(x) if( !(x) ){ __asm( "cpsid i		\n" ); for(;;); }
	 #define  DISABLE_INTERRUPT  __asm( "cpsid i		\n" );
   #define  ENABLE_INTERRUPT   __asm( "cpsie i		\n" );
	 #define  CPUREGISTER         17
		 
   #if   defined ( NXP )
      #include <LPC11xx.h>
	    #define  CPUclockOS   SystemCoreClock
   #endif
#endif
	 
#if   defined ( ARM )     
  #define  WORDSIZE    4
       // core register address
  #define SystickControlRegisterOS       ( *( ( volatile unsigned int* ) 0xE000E010 ) )
  #define SystickLoadRegisterOS          ( *( ( volatile unsigned int* ) 0xE000E014 ) )
  #define SystickCurrentValueRegisterOS  ( *( ( volatile unsigned int* ) 0xE000E018 ) )
  #define InterruptControlRegisterOS     ( *( ( volatile unsigned int* ) 0xE000ED04 ) )
  #define HandlerPriorityRegisterOS      ( *( ( volatile unsigned int* ) 0xE000ED20 ) )  //SHPR3
#endif

#define  OSCLOCK_1S       CPUclockOS 
#define  OSCLOCK_500mS    CPUclockOS/2
#define  OSCLOCK_200mS    CPUclockOS/5
#define  OSCLOCK_100mS    CPUclockOS/10     // 10 tick per second
#define  OSCLOCK_50mS     CPUclockOS/20
#define  OSCLOCK_10mS     CPUclockOS/100
#define  OSCLOCK_1mS      CPUclockOS/1000
#define  OSCLOCK_100uS    CPUclockOS/10000
#define  OSCLOCK_10K      10000
#define  OSCLOCK_100K     100000
#define  OSCLOCK_1M       1000000

           // select OS tick
#define  CLOCKOS          OSCLOCK_200mS 

           // check safety
#define  COUNTSTARTOS     10
#define  MAXLEVEL          5
#define  NULL              0x0

           // timeout infinite
#define  INFINITEOS       -1

           // event
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
char         startOS(void (*[])(void), int, int, void (*)(int), void (*)(void), void (*)(void));
void         queryReadyTableOS(char*);
void         endISRpreemptOS(void);
int          nonBlockingCallbackOS(int (*)(void));
char         errorPendSizeOS(void);
       // Stack
int          ramToPaddingOS(int, int);
int          paddingToRamOS(int, int);						
int          autoPackItemsOS(void);
int          minimumStackOS(void);
int          autoMinimumStackOS(void);
int*         minPackSizeOS(void);
int          queryResidueStackOS(void);
char         queryDangerTaskOS(void);
void         checkStackSafetyOS(int, int);
char         querySafeLevelOS(void);
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
void         postMutexOS(void);
int          pendMutexOS(int*, int);
      // Memory Management
void*        getMemoryOS(int, int);
void*        getMemoryWithPutOS(int, int);
void*        margeAddressOS(void*);
void*        dangerAddressOS(void*, int*);
void         putMemoryOS(int); 
void*        mallocOS(int);
void         freeOS(void*);
int          maxFreeMemoryOS(void);
int          minFreeMemoryOS(void);
int          queryFreeBulkNoOS(char*);
int          leakMemoryNoOS(void);
int          lackMemoryNoOS(void);
int*         leakMemoryNoAllOS(void);
int*         lackMemoryNoAllOS(void);
      // Queue
int          postQOS(int, void*);
void*        readQOS(int, int*);
void*        pendQOS(int*, char, int*, int);
char         qSleepOS(void*);
int          qReadyNumberOS(void*);
int          queryRemainItemsOS(int);
void         qRxIntFloatOS(int, void*);
void         qTxIntFloatOS(int, void*, int, char);
      // Task Loading
int*         relativeTaskLoadOS(void);
int          idleTaskLoadOS(void);
	
	


