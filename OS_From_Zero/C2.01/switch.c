__asm void PendSV(void)
{
	IMPORT stackPtr
	
	LDR R0, =stackPtr			; Put the address of stackPtr into R0
	LDR R0, [R0]					; Put the value of stackPtr, say the address of stack[1024], into R0
	STMDB R0!, {R4-R11}
	
	LDR R1, =stackPtr
	STR R0, [R1]

	ADD R4, R4, #1
	
	LDMIA R0!, {R4-R11}
	BX LR
}
