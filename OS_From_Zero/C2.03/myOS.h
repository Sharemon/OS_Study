#ifndef _MYOS_H
#define _MYOS_H

#include <stdint.h>

#define NVIC_INT_CTRL		0xE000ED04
#define NVIC_PENDSVSET	0x10000000
#define NVIC_SYSPRI2		0xE000ED22
#define NVIC_PENDSV_PRI	0x000000FF

#define MEM32(addr)			*(volatile unsigned long *)(addr)
#define MEM8(addr)			*(volatile unsigned char *)(addr)


typedef uint32_t TaskStack_t;

typedef struct _Task_t
{
	TaskStack_t * stackPtr;
} Task_t;	
// Actually, this structure is similiar to TCB in ucos.
// The task function is always called "task"
// The data saving memory is called "stack"

extern Task_t * curTask;
extern Task_t * nxtTask;

void TaskRun(void);
void TaskSwitch(void);

#endif
