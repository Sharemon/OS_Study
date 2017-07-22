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
#define MYOS_TASK_STATE_SUSPENDED			(1<<2)
#define MYOS_TASK_STATE_DELETED				(1<<3)


typedef uint32_t taskStack_t;

typedef struct _task_t
{
	taskStack_t * stackPtr;
	node_t linkNode;
	uint32_t delayTicks;
	uint32_t prio;
	node_t delayNode;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;
	
	void (*clean) (void * param);
	void * cleanParam;
	uint8_t requestDeleteFlag;
} task_t;	
// Actually, this structure is similiar to TCB in ucos.
// The task function is always called "task"
// The data saving memory is called "stack"


typedef struct _taskInfo_t
{
	uint32_t delayTicks;
	uint32_t prio;
	uint32_t state;
	uint32_t slice;
	uint32_t suspendCount;
}taskInfo_t;


extern task_t * curTask;
extern task_t * nxtTask;

uint32_t TaskEnterCritical (void);
void TaskExitCritical (uint32_t status);

void TaskRun(void);
void TaskSwitch(void);

void TaskSchedInit (void);
void TaskSchedDisable (void);
void TaskSchedEnable (void);
void TaskSchedRdy (task_t * task);
void TaskSchedUnRdy (task_t * task);
void TaskSched (void);
void TimeTaskWait (task_t * task, uint32_t ticks);
void TimeTaskWakeUp (task_t * task);
void TaskSystemTickHandler(void);
void TaskDelay (uint32_t delay);
void TaskInit (task_t * task, void (*entry)(void *), void * param, taskStack_t * stack, uint32_t prio);
void SetSysTickPeriod (uint32_t ms);
void InitApp(void);
void OSdelay(uint32_t delay);
void SysTick_Handler(void);
void SysTickInit(uint32_t n);
void TaskSuspend (task_t * task);
void TaskWakeUp (task_t * task);
void TimeTaskRemove (task_t * task);
void TaskSchedRemove (task_t * task);
void TaskSetCleanCallFunc(task_t * task, void (*clean)(void * param), void * param);
void TaskForceDelete(task_t * task);
void TaskRequestDelete(task_t * task);
uint8_t TaskIsRequestDeleted(void);
void TaskDeleteSelf(void);
void TaskGetInfo(task_t * task, taskInfo_t * taskInfo);

#endif
