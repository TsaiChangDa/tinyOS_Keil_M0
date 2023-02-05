
#include "OS.h"

int xx(void)
{
	  return 0;
}


int    *pint;
char    carray[3];
int a = 0;
int b = 5;
float   *pfloat;

void task0(void)  
{	
    int n[]={0, -1}; 
		int items;
		
	  while(1)
    { 
				 queryReadyTableOS(carray);
			   endISRpreemptOS();
			   nonBlockingCallbackOS(xx);
			   errorPendSizeOS();

			   ramToPaddingOS(2000, 2);
			   paddingToRamOS(20, 2);
         autoPackItemsOS();
				 minimumStackOS();
			   autoMinimumStackOS();
				 minPackSizeOS();
				 queryResidueStackOS();
         queryDangerTaskOS();
         checkStackSafetyOS(1, 1);
				 querySafeLevelOS();

				 delayTickOS( 8 );
			   delayTimeOS(0, 0, 0, 10);				
				 delayUntilEqualOS(&a, &b);
			   delayUntilTrueOS(&a);
				 
				 postSemOS(0);
				 pendSemOS(n, 5);
								 
			   postMailOS(0, 0x0);	
			   pendMailOS(n, 0x0, 0, 5);
			   readMailOS(0, 0);
				 
			   postFlagOS(0, 0x0 , FLAG_SET);
				 pendFlagOS(n, 0x0, FLAG_MATCH_ANY, 5 );
			   queryPublicFlagOS(0);
			   checkPublicFlagBitOS(0, 0);
	
			   pendMutexOS(n, 5);
			   postMutexOS();
			
		     getMemoryOS(11, 0);
			   getMemoryWithPutOS(1, 1);
			   margeAddressOS(0x0);
			   dangerAddressOS(0x0, 0x0);
				 putMemoryOS(0);
			   mallocOS(8);
			   freeOS(pfloat);
			   maxFreeMemoryOS();
				 minFreeMemoryOS();
				 queryFreeBulkNoOS(carray);
			   leakMemoryNoOS();
			   lackMemoryNoOS();
         leakMemoryNoAllOS();
         lackMemoryNoAllOS();

         postQOS(0, 0x0);  
         pendQOS(n, 1, &items, 5);
			   readQOS(0, &items);
			   qReadyNumberOS(pint);
			   qSleepOS(pint);
         queryRemainItemsOS(0);
         qRxIntFloatOS(0, 0x0);
	       qTxIntFloatOS(0, 0x0, 4, 2);
	
			   relativeTaskLoadOS();
         idleTaskLoadOS();
         ASSERTOS(0);	
    } 
} 

void (*taskName[])(void)={task0};

int main(void)  
{
	   int startTaskIndex;
	   int arraySize;
	
	   arraySize = sizeof(taskName) / sizeof(taskName[0]);
     startTaskIndex = 0;
     startOS(taskName, arraySize, startTaskIndex, NULL, NULL, NULL); 
} 


