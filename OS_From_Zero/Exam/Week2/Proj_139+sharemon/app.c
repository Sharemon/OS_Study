#include "myOS.h"

//---------------- TASK ------------------
task_t task1;
task_t task2;
task_t task3;

taskStack_t task1Env[1024];
taskStack_t task2Env[1024];
taskStack_t task3Env[1024];


int task1Flag;
// ===== Test code for single linked list =====
list_t testSigleList;
node_t testNode[5];
// ===== Test code for single linked list =====
void Task1(void * param)
{
	int i;
	
	SysTickInit(1);

	// ===== Test code for single linked list =====
	ListInit(&testSigleList);
	
	for (i=0; i<5; i++)
	{
		NodeInit(&testNode[i]);
		ListAddFirst(&testSigleList, &testNode[i]);
	}
	
	for (i=0; i<5; i++)
	{
		ListRemoveFirst(&testSigleList);
	}
	// ===== Test code for single linked list =====
	
	for(;;)
	{		
		task1Flag = 0;
		OSdelay(4);
		task1Flag = 1;
		OSdelay(4);
	}
}


int task2Flag;
void Task2(void * param)
{
	
	for(;;)
	{
		task2Flag = 0;
		OSdelay(7);
		task2Flag = 1;
		OSdelay(7);
	}
}

int task3Flag;
void Task3(void * param)
{
	
	for(;;)
	{
		task3Flag = 0;
		OSdelay(6);
		task3Flag = 1;
		OSdelay(6);
	}
}


void InitApp(void)
{
	TaskInit(&task1, Task1, (void *)0x11111111, &task1Env[1024], 0);
	TaskInit(&task2, Task2, (void *)0x22222222, &task2Env[1024], 1);
	TaskInit(&task3, Task3, (void *)0x33333333, &task3Env[1024], 2);
}

//---------------- TASK ------------------
