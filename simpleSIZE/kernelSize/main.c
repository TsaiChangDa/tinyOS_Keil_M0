
#include "OS.h"

void task0(void)  
{  
   while(1) 
   { 
   } 
} 
void task1(void)  
{
   while(1) 
   { 
   } 
} 
void task2(void)  
{  
   while(1) 
   { 
   } 
} 
void task3(void)  
{
   while(1) 
   { 
   } 
} 
void task4(void)  
{  
   while(1) 
   { 
   } 
} 
void task5(void)  
{
   while(1) 
   { 
   } 
} 
void task6(void)  
{  
   while(1) 
   { 
   } 
} 
void task7(void)  
{
   while(1) 
   { 
   } 
}

void (*taskName[])(void)={task0,task1, task2,task3,task4,task5, task6,task7};

	
      // ErrorCode : 1- TaskCountOS != TASKSIZE+1
int main(void)  
{
	   int startTaskIndex;
	   int arraySize;
	
	   arraySize = sizeof(taskName) / sizeof(taskName[0]);
     startTaskIndex = 0;
     startOS(taskName, arraySize, startTaskIndex, NULL, NULL, NULL); 
	 
} // main






