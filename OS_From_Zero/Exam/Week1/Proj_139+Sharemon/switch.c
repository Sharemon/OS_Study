#include "myOS.h"
#include "ARMCM3.h"

__asm void PendSV_Handler(void)
{													; before enter function, PSP will store all needed data (hardware think) into stack
	IMPORT curTask
	IMPORT nxtTask
	IMPORT errno
	
	MRS R0, PSP
	CBZ R0, PendSVHandler_firstRun
	
	LDR R1, =errno					; R1 = &errno
	LDR R1, [R1]						; R1 = errno
	STMDB R0!, {R1, R4-R11}	; save R1(errno) after R4 to stack
	
	LDR R1, =curTask
	LDR R1, [R1]
	STR R0, [R1]
	
PendSVHandler_firstRun
	LDR R0, =curTask				; R0 = &curTask
	LDR R1, =nxtTask				; R1 = &nxtTask
	LDR R1, [R1]						; R1 = nxtTask
	STR R1, [R0]						; *(&curTask) = nxtTask
	
	LDR R1, [R1]						; R1 = *nxtTask (nxtTask.stackPtr)
	LDMIA R1!, {R2, R4-R11}	; Load R2 (errno), R4-R11 from stack
	
	MSR PSP, R1							; PSP = R1, 
													; **important**
													; now the PSP point to the one cell of task stack
													; so after switch, hardware will pop and push data into this stack
													; but not the original one
	
	LDR R1, =errno					; R1 = &errno
	STR R2, [R1]						; errno = R2, restore errno
	
	ORR LR, LR, #0x04				; set code running on user level to match PSP
	
	BX LR										; after return, PSP will get back the data saved by hardware 
}

void TaskRun(void)
{
	__set_PSP(0);
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}


void TaskSwitch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
