


	  // task
#define  TASKSIZE           2
#define  STACKRAMBYTES   2500

    // event and service
#define  PENDSIZE           6
#define  MEMORYPOOLBYTES   96
#define  GETMEMORYSIZE      0
#define  QSIZE              1    
#define  QBYTES             8	
#define  MAILSIZE           1
#define  FLAGSIZE           1	
#define  MUTEXSIZE          1
#define  UNTILSIZE          0

    // padding part of task stack(selective)
int PaddingOS[TASKSIZE+1] = {0};  
#define  PADDINGIDLE       0      // idleTaskOS() padding





