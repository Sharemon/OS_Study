#include <stdio.h>
#include "myOS.h"

	
Task_t * curTask;
Task_t * nxtTask;
Task_t * taskTable[2];

Task_t tcbtask1;
Task_t tcbtask2;

TaskStack_t task1Env[1024];
TaskStack_t task2Env[1024];

void delay(int n)
{
	while(--n);
}

void trigPendSVC()
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
/*
	int i,j,k,m;//,n,p;
	i++;j++;k++;//m++;n++;p++;
	g_variable2=i;g_variable2=j;g_variable2=k;//g_variable2=m;g_variable2=n;g_variable2=p;
*/
int g_variable2;
void TaskSched()
{
	int i1=10,i2=10,i3=10,i4=10,i5=10,i6=10,i7=10,i8=10,i9=10,i10=10;
	int i11=10,i12=10,i13=10,i14=10,i15=10,i16=10,i17=10,i18=10,i19=10,i20=10;
	delay(i1);delay(i2);delay(i3);delay(i4);delay(i5);
	delay(i6);delay(i7);delay(i8);delay(i9);delay(i10);
	delay(i11);delay(i12);delay(i13);delay(i14);delay(i15);
	delay(i16);delay(i17);delay(i18);delay(i19);delay(i20);
}


void TaskInit(Task_t * tTaskPtr, void (*entry)(void *), 
		void * param, TaskStack_t * stack)
{
	// will be saved by hardware
	// will never store local variables. After return, the local variables will be released
	*(--stack) = (unsigned long) (1<<24);
	*(--stack) = (unsigned long) entry;
	*(--stack) = (unsigned long) 0x14;
	*(--stack) = (unsigned long) 0x12;
	*(--stack) = (unsigned long) 0x03;
	*(--stack) = (unsigned long) 0x02;
	*(--stack) = (unsigned long) 0x01;
	*(--stack) = (unsigned long) param;
	
	// need to be saved by user
	*(--stack) = (unsigned long) 0x11;
	*(--stack) = (unsigned long) 0x10;
	*(--stack) = (unsigned long) 0x09;
	*(--stack) = (unsigned long) 0x08;
	*(--stack) = (unsigned long) 0x07;
	*(--stack) = (unsigned long) 0x06;
	*(--stack) = (unsigned long) 0x05;
	*(--stack) = (unsigned long) 0x04;
	
	tTaskPtr->stackPtr = stack;
}

int taks1Flag;
int g_variable;
void task1(void * param)
{
	int i1=0,i2=0,i3=0,i4=0,i5=0,i6=0,i7=0,i8=0,i9=0,i10=0;
	for(;;)
	{
		i1++;i2++;i3++;i4++;i5++;
		i6++;i7++;i8++;i9++;i10++;
		g_variable=i1;g_variable=i2;g_variable=i3;g_variable=i4;g_variable=i5;
		g_variable=i6;g_variable=i7;g_variable=i8;g_variable=i9;g_variable=i10;
		TaskSched();
	}
}

int taks2Flag;
void task2(void * param)
{
	for(;;)
	{
		taks2Flag = 0;
		delay(100);
		taks2Flag = 1;
		delay(100);
		TaskSched();
	}
}

extern void PendSV(void);

int main(void)
{
	// Initial task
	TaskInit(&tcbtask1, task1, (void *)0x11111111, &task1Env[1024]);
	TaskInit(&tcbtask2, task2, (void *)0x22222222, &task2Env[1024]);
	
	taskTable[0] = &tcbtask1;
	taskTable[1] = &tcbtask2;
	
	nxtTask = taskTable[0];
	
	TaskRun();
	
	return 0;		// will never be called
}
