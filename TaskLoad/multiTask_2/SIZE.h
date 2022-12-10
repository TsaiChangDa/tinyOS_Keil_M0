


	  // task
#define  TASKSIZE           10 
#define  STACKRAMBYTES   2500

    // event and service
#define  PENDSIZE           1
#define  MEMORYPOOLBYTES   16
#define  GETMEMORYSIZE      0
#define  QSIZE              1    
#define  QBYTES             8	
#define  MAILSIZE           0
#define  FLAGSIZE           0	
#define  MUTEXSIZE          0
#define  UNTILSIZE          0

    // padding part of task stack(selective)
int PaddingOS[TASKSIZE+1] = {0}; 
#define  PADDINGIDLE       0      // idleTaskOS() padding



