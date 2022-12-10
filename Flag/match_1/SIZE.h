


/*	
PEND MAIL      3     // task0,task6,task7
PEND FLAG      5     // task1,task2,task3,task4,task5
 */ 

	  // task
#define  TASKSIZE          9 
#define  STACKRAMBYTES     2500

    // event and service
#define  PENDSIZE           8
#define  MEMORYPOOLBYTES    0
#define  GETMEMORYSIZE      0
#define  QSIZE              0    
#define  QBYTES             0	
#define  MAILSIZE           3
#define  FLAGSIZE           2	
#define  MUTEXSIZE          0
#define  UNTILSIZE          0

    // padding part of task stack(selective)
int PaddingOS[TASKSIZE+1] = {0};  
#define  PADDINGIDLE       0      // idleTaskOS() padding





