#include "myOS.h"

//---------------- TASK ------------------
Task_t task1;
Task_t task2;
Task_t task3;

TaskStack_t task1Env[1024];
TaskStack_t task2Env[1024];
TaskStack_t task3Env[1024];


int taks1Flag;
void Task1(void * param)
{
	SysTickInit(1);

	for(;;)
	{		
		taks1Flag = 0;
		//OSdelay(1);
		TaskSuspend(curTask);
		taks1Flag = 1;
		//OSdelay(1);
		TaskSuspend(curTask);
	}
}

void delay(uint32_t n)
{
	while(--n);
}

int taks2Flag;
void Task2(void * param)
{
	for(;;)
	{

		taks2Flag = 0;
		OSdelay(1);
		TaskWakeUp(&task1);
		taks2Flag = 1;
		OSdelay(1);
		TaskWakeUp(&task1);
	}
}

int taks3Flag;
void Task3(void * param)
{
	for(;;)
	{

		taks3Flag = 0;
		OSdelay(1);
		taks3Flag = 1;
		OSdelay(1);
	}
}


void InitApp(void)
{
	TaskInit(&task1, Task1, (void *)0x11111111, &task1Env[1024], 0);
	TaskInit(&task2, Task2, (void *)0x22222222, &task2Env[1024], 1);
	TaskInit(&task3, Task3, (void *)0x33333333, &task3Env[1024], 1);
}

//---------------- TASK ------------------