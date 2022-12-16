#include <LPC11xx.h>

#define  ASSERTOS(x) if( !(x) ){ __ASM{cpsid i}; for(;;); }

#define  OSCLOCK_1S      SystemCoreClock  // defined in system_LPC11xx.c
#define  OSCLOCK_500mS   SystemCoreClock/2
#define  OSCLOCK_200mS   SystemCoreClock/5
#define  OSCLOCK_100mS   SystemCoreClock/10   // 10 tick per second
#define  OSCLOCK_50mS    SystemCoreClock/20
#define  OSCLOCK_10mS    SystemCoreClock/100
#define  OSCLOCK_1mS     SystemCoreClock/1000
#define  OSCLOCK_100uS   SystemCoreClock/10000

#define  OSCLOCK_10K     10000
#define  OSCLOCK_100K    100000
#define  OSCLOCK_1M      1000000

#define  CLOCKOS         OSCLOCK_100mS  // number of core clock per OS tick clock

#define  INFINITEOS      -1
	
#define SystickControlRegisterOS       ( *( ( volatile unsigned int* ) 0xE000E010 ) )
#define SystickLoadRegisterOS          ( *( ( volatile unsigned int* ) 0xE000E014 ) )
#define SystickCurrentValueRegisterOS  ( *( ( volatile unsigned int* ) 0xE000E018 ) )
#define InterruptControlRegisterOS     ( *( ( volatile unsigned int* ) 0xE000ED04 ) )
#define HandlerPriorityRegisterOS      ( *( ( volatile unsigned int* ) 0xE000ED20 ) )  //SHPR3

            // application function
char startOS(void (*[])(void), int, int, int);
void queryReadyTableOS(char*);
int  findOptimalPaddingOS(void);
void delayTickOS(int);
void delayTimeOS(int, int, int, int);
void postSemOS(char);
void pendSemOS(char, int);
int  lowPowerModeOS(int*);

