
#include "PORT.h"

#if   defined ( KEIL )
#if   defined ( CM0 )

int findLeastBitOS(unsigned int Table) 
{
	__asm (
          " B entry \n"     
"return1: \n"
"	 MOV  R0, R3    \n"
"	 BX   LR \n"
"entry: \n"
	  " MOVS R3, #0 	\n"
"loop1: \n "
"	  LSRS R0,R0,#1 \n "
"	  BCS  return1  \n "
"	 ADDS  R3, R3, #1 \n "
" CMP  R3, #32 \n "
"	 BLT  loop1 \n "
  "       B return1 \n"
);
        return 0;
}

int interruptNumberOS(void)
{ __asm (
   	"MRS  R0, IPSR \n"
	 "  BX   LR \n");
  return 0;



}



 void setPSPOS(unsigned int topPointer)
{
	__asm(  
     " THUMB \n"
      "MSR  PSP, R0 \n"
      "	   BX   LR \n"
  );	
}


void setCONTROLOS(unsigned int usePSP)
{

  __asm (
         "MSR  CONTROL, R0 \n"
	 "   ISB \n"
	 "   BX   LR \n"
  	);
}



extern unsigned int* CurrentTaskOS;      // address of sp(the entry of CPU registers stored in ram stack)

 

#endif
#endif

