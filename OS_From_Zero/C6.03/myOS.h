#ifndef _MYOS_H
#define _MYOS_H

#include <stdint.h>
#include "list.h"
#include "config.h"
#include "task.h"
#include "event.h"
#include "sem.h"

#define NVIC_INT_CTRL		0xE000ED04
#define NVIC_PENDSVSET	0x10000000
#define NVIC_SYSPRI2		0xE000ED22
#define NVIC_PENDSV_PRI	0x000000FF

#define MEM32(addr)			*(volatile unsigned long *)(addr)
#define MEM8(addr)			*(volatile unsigned char *)(addr)



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
void SetSysTickPeriod (uint32_t ms);
void InitApp(void);
void OSdelay(uint32_t delay);
void SysTick_Handler(void);
void SysTickInit(uint32_t n);
void TimeTaskRemove (task_t * task);
void TaskSchedRemove (task_t * task);


#endif
