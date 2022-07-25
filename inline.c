
#include "PORT.h"

#if   defined ( KEIL )
#if   defined ( CM0 )

__asm int findLeastBitOS(unsigned int Table)
{
	MOVS R3, #0
	
loop1
	LSRS R0,R0,#1
	BCS  return1
	ADDS  R3, R3, #1
	CMP  R3, #32
	BLT  loop1

return1
	MOV  R0, R3  
	BX   LR
}

__asm int interruptNumberOS(void)  
{
    MRS  R0, IPSR
	  BX   LR

}


__asm void setPSPOS(unsigned int topPointer)
{
	  PRESERVE8
	  THUMB
	
    MSR  PSP, R0
	  BX   LR	
}


__asm void setCONTROLOS(unsigned int usePSP)
{
		PRESERVE8
	  THUMB
	
    MSR  CONTROL, R0
	  ISB
	  BX   LR	
}



__asm void PendSV_Handler(void)
{	
//PUSH 
	MRS	R0, PSP  
	SUBS R0, #32
	STMIA R0!,{R4-R7} 
	MOV	R4, R8
	MOV	R5, R9
	MOV	R6, R10
	MOV	R7, R11
	STMIA R0!,{R4-R7}   
	SUBS R0, #32       
	IMPORT  CurrentTaskOS  
	LDR	R2, =CurrentTaskOS
	LDR	R1, [R2]         
	STR	R0, [R1]        
	
//POP	
	IMPORT  NextTaskOS   
	LDR	R2, =NextTaskOS
	LDR	R1, [R2]
	LDR	R0, [R1]        
	LDMIA R0!,{R4-R7}  
	LDMIA R0!,{R1-R3}  
	MOV	R8, R1
	MOV	R9, R2
	MOV	R10, R3
	LDMIA R0!,{R1}	
	MOV	R11, R1        

	MSR	PSP, R0      

	LDR R3, =0xFFFFFFFD  

	BX	R3       
}


#endif
#endif

