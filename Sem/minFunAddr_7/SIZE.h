


	  // task
#define  TASKSIZE           4 
//#define  STACKRAMBYTES   2500
#define  STACKRAMBYTES   0x2d8   

    // event and service
#define  PENDSIZE           3
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
int PaddingOS[TASKSIZE+1] = {0x2b, 0x12, 0x12,0x12}; 
#define  PADDINGIDLE       0      // idleTaskOS() padding



