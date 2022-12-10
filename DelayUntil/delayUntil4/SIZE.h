


	  // task
#define  TASKSIZE          3 
#define  STACKRAMBYTES     2500

    // event and service
#define  PENDSIZE           0
#define  MEMORYPOOLBYTES    0
#define  GETMEMORYSIZE      0
#define  QSIZE              0    
#define  QBYTES             0	
#define  MAILSIZE           0
#define  FLAGSIZE           0	
#define  MUTEXSIZE          0
#define  UNTILSIZE          2

    // padding part of task stack(selective)
int PaddingOS[TASKSIZE+1] = {0};  // 172
#define  PADDINGIDLE       0      // idleTaskOS() padding

