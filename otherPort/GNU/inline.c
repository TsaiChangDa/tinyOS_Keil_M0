
#include "PORT.h"

__attribute__ ((naked, optimize("-fno-stack-protector")))
int findLeastBitOS(unsigned int Table)
{

	asm volatile(
			"  .syntax unified \n"
			"  .cpu cortex-m0  \n"
			" .fpu softvfp     \n"
			" .thumb           \n"

			"movs r3, #0		\n"

		"loop1:					\n"
			"lsrs r0,r0,#1		\n"
			"bcs  loop2		\n"
			"adds  r3, r3, #1	\n"
			"cmp  r3, #32		\n"
			"blt  loop1			\n"

		"loop2:					\n"
			"mov  r0, r3		\n"
 	        "BX   LR \n" 

		);

	return 0;
}


__attribute__ ((naked, optimize("-fno-stack-protector")))
int interruptNumberOS(void)
{
	asm volatile(

			"MRS R0, IPSR		\n"
  	        "ISB				\n"
 	        "DSB				\n"
	        "BX   LR           \n" 
	);

	return 0;
}


__attribute__ ((naked, optimize("-fno-stack-protector")))
unsigned int setPSPOS(unsigned int top)
{

	asm volatile(
	"	    MSR  PSP, R0  \n"
	"	    BX   LR \n"
		);
   return 0;
}

__attribute__ ((naked, optimize("-fno-stack-protector")))
void setCONTROLOS(unsigned int usePSP)
{
	asm volatile(

			"MSR  CONTROL,R0	\n"
			"ISB				\n"
	        "BX   LR            \n"
		);

}


__attribute__ ((naked, optimize("-fno-stack-protector")))
void PendSV_Handler(void)
{

    asm volatile(
			"  .syntax unified \n"
			"  .cpu cortex-m0  \n"
			"  .fpu softvfp    \n"
			"  .thumb          \n"

			"  MRS	R0, PSP			          \n"
			"  SUBS R0, #16			          \n"
			"  STMIA R0!,{R4-R7}	          \n"
			"  MOV	R4, R8			          \n"
			"  MOV	R5, R9			          \n"
			"  MOV	R6, R10			          \n"
			"  MOV	R7, R11			          \n"
			"  SUBS R0, #32			          \n"
			"  STMIA R0!,{R4-R7}	          \n"
			"  SUBS R0, #16			          \n"

			"  LDR	R2, =CurrentTaskOS        \n"
			"  LDR	R1, [R2]			      \n"
			"  STR	R0, [R1]		          \n"

			"  LDR	R2, =NextTaskOS		      \n"
			"  LDR	R1, [R2]		          \n"
			"  LDR	R0, [R1]		          \n"
			"  LDMIA R0!,{R4-R7}	          \n"
			"  MOV	R8, R4			          \n"
			"  MOV	R9, R5			          \n"
			"  MOV	R10, R6			          \n"
			"  MOV	R11, R7			          \n"
			"  LDMIA R0!,{R4-R7}		      \n"
			"  MSR	PSP, R0			          \n"

			"  LDR R3, =0xFFFFFFFD	          \n"

			"  BX	R3				          \n"

	        "	.align 4					\n"

    );

}



