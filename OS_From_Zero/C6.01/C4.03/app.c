#include "myOS.h"

//---------------- TASK ------------------
task_t task1;
task_t task2;
task_t task3;
task_t task4;

taskStack_t task1Env[1024];
taskStack_t task2Env[1024];
taskStack_t task3Env[1024];
taskStack_t task4Env[1024];


int task1Flag;

void task1DestroyFunc(void * param)
{
	task1Flag = 0;
}

void Task1(void * param)
{
	SysTickInit(1);
	
	TaskSetCleanCallFunc(&task1, task1DestroyFunc, (void *)0);

	for(;;)
	{		
		task1Flag = 0;
		OSdelay(1);
		task1Flag = 1;
		OSdelay(1);
	}
}

void delay(uint32_t n)
{
	while(--n);
}

int task2Flag;
void Task2(void * param)
{
	uint8_t deleteTask1Flag = 1;
	
	for(;;)
	{
		task2Flag = 0;
		OSdelay(1);
		task2Flag = 1;
		OSdelay(1);
		
		if (deleteTask1Flag)
		{
			deleteTask1Flag = 0;
			TaskForceDelete(&task1);
		}
	}
}

int task3Flag;
void Task3(void * param)
{
	for(;;)
	{
		task3Flag = 0;
		OSdelay(1);
		task3Flag = 1;
		OSdelay(1);
		
		if (TaskIsRequestDeleted())
		{
			task3Flag = 0;
			TaskDeleteSelf();
		}
	}
}

int task4Flag;
void Task4(void * param)
{
	for(;;)
	{
		task4Flag = 0;
		OSdelay(1);
		task4Flag = 1;
		OSdelay(1);
		
		TaskRequestDelete(&task3);
	}
}


void InitApp(void)
{
	TaskInit(&task1, Task1, (void *)0x11111111, &task1Env[1024], 0);
	TaskInit(&task2, Task2, (void *)0x22222222, &task2Env[1024], 1);
	TaskInit(&task3, Task3, (void *)0x33333333, &task3Env[1024], 0);
	TaskInit(&task4, Task4, (void *)0x33333333, &task4Env[1024], 1);
}

//---------------- TASK ------------------
