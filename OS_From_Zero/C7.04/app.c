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

mBox_t mbox1;
mBox_t mbox2;

void * mBoxMsgBuffer1[20];
void * mBoxMsgBuffer2[20];

uint32_t msg[20];

int task1Flag;

void Task1(void * param)
{
	int i;
	
	SysTickInit(10);

	MBoxInit(&mbox1, mBoxMsgBuffer1, 20);
	for(;;)
	{		
		for(i=0;i<20;i++)
    {
			msg[i] = i;
			MBoxNotify(&mbox1, ((uint32_t *)msg)+i, MBoxSendFront); 
    }
		
		OSdelay(100);
		
		for(i=0;i<20;i++)
    {
			msg[i] = i;
			MBoxNotify(&mbox1, msg+i, MBoxSendNormal); 
    }
		
		OSdelay(100);
		
		task1Flag = 0;
		OSdelay(1);
		task1Flag = 1;
		OSdelay(1);
	}
}



int task2Flag;
void Task2(void * param)
{
	uint32_t err;
	void * msg;
	
	for(;;)
	{
		err = MBoxWait(&mbox1, &msg, 0);
		if (err == ErrorNoError)
		{
			task2Flag = *((uint32_t *)msg);
			OSdelay(1);
		}
		
		task2Flag = 0;
		OSdelay(1);
		task2Flag = 1;
		OSdelay(1);

	}
}

int task3Flag;
void Task3(void * param)
{
	void * msg;
	
	MBoxInit(&mbox2, mBoxMsgBuffer2, 20);
	for(;;)
	{
		MBoxWait(&mbox2, &msg, 100);
		task3Flag = 0;
		OSdelay(1);
		task3Flag = 1;
		OSdelay(1);
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
