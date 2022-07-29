
#include "PORT.h"

#if   defined ( KEIL )
#if   defined ( CM0 )

int findLeastBitOS(unsigned int Table) 
{
	__asm(
	"  MOVS R3, #0 \n"	
  "loop1: \n"
	"  LSRS R0,R0,#1 \n"
	"  BCS  return1 \n"
	" ADDS  R3, R3, #1 \n"
	" CMP  R3, #32 \n"
	" BLT  loop1 \n"
  "return1: \n"
	" MOV  R0, R3 \n"  
	" BX   LR \n"
	);
	return 0;
}

int interruptNumberOS(void)
{
    __asm(
   "	MRS  R0, IPSR \n"
	 "  BX   LR \n"
	);
	return 0;

}


void setPSPOS(unsigned int topPointer)
{
	  __asm(
//	  "  PRESERVE8 \n"
//	  " THUMB \n"
    " MSR  PSP, R0 \n"
	  " BX   LR \n"
  );	
}


void setCONTROLOS(unsigned int usePSP)
{
		__asm(
	 // "  PRESERVE8 \n"
	 // "  THUMB \n"
    "  MSR  CONTROL, R0 \n"
	  "  ISB \n"
	  "  BX   LR \n"
  );	
}



void PendSV_Handler(void)
{	
//PUSH 
	__asm(
	"  MRS	R0, PSP \n"  
	"  SUBS R0, #32  \n"
	"  STMIA R0!,{R4-R7} \n" 
	"  MOV	R4, R8 \n"
	"  MOV	R5, R9  \n"
	"   MOV	R6, R10 \n"
	"  MOV	R7, R11 \n"
	" STMIA R0!,{R4-R7} \n"   
	"  SUBS R0, #32  \n"       
//	"  IMPORT  CurrentTaskOS \n"  
	"  LDR	R2, =CurrentTaskOS \n"
	"  LDR	R1, [R2] \n"         
	"  STR	R0, [R1]  \n"        	
//	"  IMPORT  NextTaskOS \n"   
	"  LDR	R2, =NextTaskOS \n"
	"  LDR	R1, [R2] \n"
	"  LDR	R0, [R1] \n"        
	"  LDMIA R0!,{R4-R7} \n"  
	"  LDMIA R0!,{R1-R3}  \n"  
	"   MOV	R8, R1  \n"
	"  MOV	R9, R2  \n"
	"  MOV	R10, R3 \n"
	"  LDMIA R0!,{R1}  \n"	
	"  MOV	R11, R1  \n"        
	"  MSR	PSP, R0  \n"      
	"  LDR R3, =0xFFFFFFFD  \n"  
	"   BX	R3  \n"       
	);
}


#endif
#endif

