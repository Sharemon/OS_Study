#ifndef _MYOS_H
#define _MYOS_H

#include <stdint.h>
#include "tLib.h"
#include "tConfig.h"

#define NVIC_INT_CTRL		0xE000ED04
#define NVIC_PENDSVSET	0x10000000
#define NVIC_SYSPRI2		0xE000ED22
#define NVIC_PENDSV_PRI	0x000000FF

#define MEM32(addr)			*(volatile unsigned long *)(addr)
#define MEM8(addr)			*(volatile unsigned char *)(addr)

#define MYOS_TASK_STATE_RDY 					0
#define MYOS_TASK_STATE_DELAYED				(1<<1)


typedef uint32_t TaskStack_t;

typedef struct _Task_t
{
	TaskStack_t * stackPtr;
	node_t linkNode;
	uint32_t delayTicks;
	uint32_t prio;
	node_t delayNode;
	uint32_t state;
	uint32_t slice;
} Task_t;	
// Actually, this structure is similiar to TCB in ucos.
// The task function is always called "task"
// The data saving memory is called "stack"

extern Task_t * curTask;
extern Task_t * nxtTask;

uint32_t TaskEnterCritical (void);
void TaskExitCritical (uint32_t status);

void TaskRun(void);
void TaskSwitch(void);

void TaskSchedInit (void);
void TaskSchedDisable (void);
void TaskSchedEnable (void);
void TaskSchedRdy (Task_t * task);
void TaskSchedUnRdy (Task_t * task);
void TaskSched (void);
void TimeTaskWait (Task_t * task, uint32_t ticks);
void TimeTaskWakeUp (Task_t * task);
void TaskSystemTickHandler (void);
void TaskDelay (uint32_t delay);
void TaskInit (Task_t * task, void (*entry)(void *), void * param, TaskStack_t * stack, uint32_t prio);
void SetSysTickPeriod (uint32_t ms);

#endif
