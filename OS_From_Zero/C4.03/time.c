#include "myOS.h"

void OSdelay(uint32_t delay)
{
	uint32_t status  = TaskEnterCritical();
	
	TimeTaskWait(curTask, delay);
	
	TaskSchedUnRdy(curTask);
	
	TaskExitCritical(status);
	
	TaskSched();
}

