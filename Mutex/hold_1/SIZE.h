


	  // task
#define  TASKSIZE           3 
#define  STACKRAMBYTES   2000

    // event and service
#define  PENDSIZE           2
#define  MEMORYPOOLBYTES    0
#define  GETMEMORYSIZE      0
#define  QSIZE              0    
#define  QBYTES             0	
#define  MAILSIZE           0
#define  FLAGSIZE           0	
#define  MUTEXSIZE          1
#define  UNTILSIZE          0

    // padding part of task stack(selective)
int PaddingOS[TASKSIZE+1] = {0}; 
#define  PADDINGIDLE       0      // idleTaskOS() padding



