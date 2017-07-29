#include "myOS.h"


void FlagGroupInit(flagGroup_t * flagGroup, uint32_t flags)
{
	EventInit(&flagGroup->event, EventTypeFlagGroup);
	flagGroup->flag = flags;
}


static uint32_t FlagGroupCheckAndConsume(flagGroup_t * flagGroup, uint32_t type, uint32_t * flag)
{
	uint32_t srcFlag = *flag;
	uint32_t isSet = type & FLAGGROUP_SET;
	uint32_t isAll = type & FLAGGROUP_ALL;
	uint32_t isConsume = type & FLAGGROUP_CONSUME;
	
	uint32_t calFlag = isSet ? (flagGroup->flag & srcFlag) : (~flagGroup->flag & srcFlag);
	
	if (((isAll != 0) && (calFlag == srcFlag)) || ((isAll == 0) && (calFlag != 0)))
	{
		if (isConsume)
		{
			if (isSet)
			{
				flagGroup->flag &= ~calFlag;
			}
			else
			{
				flagGroup->flag |= calFlag;
			}
		}
		
		*flag = calFlag;
		return ErrorNoError;
		
	}
	
	*flag = calFlag;
	return ErrorResourceUnavaliable;
	
}


uint32_t FlagGroupWait(flagGroup_t * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag, uint32_t waitTicks)
{
	uint32_t result;
	uint32_t flags = requestFlag;
	
	uint32_t status = TaskEnterCritical();
  
  if ((result = FlagGroupCheckAndConsume(flagGroup, waitType, &flags)) != ErrorNoError)
	{
		curTask->waitFlagType = waitType;
		curTask->eventFlag = requestFlag;
		EventWait(&flagGroup->event, curTask, (void *)0, EventTypeFlagGroup, waitTicks);
		
		TaskExitCritical(status);
		
		TaskSched();
		
		*resultFlag = curTask->eventFlag;
		result = curTask->waitEventResult;
	}
	else
	{
		*resultFlag = flags;
		TaskExitCritical(status);
	}
  
  return result;
}



uint32_t FlagGroupNoWaitGet(flagGroup_t * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag)
{
	uint32_t result;
	uint32_t flags = requestFlag;
	
	uint32_t status = TaskEnterCritical();
  
  result = FlagGroupCheckAndConsume(flagGroup, waitType, &flags);
  
  TaskExitCritical(status);
	
	*resultFlag = flags;
	return result;
}


void FlagGroupNotify(flagGroup_t * flagGroup, uint8_t isSet, uint32_t flag)
{
	list_t waitList = flagGroup->event.waitList;
	node_t  * node = (node_t *)0;
	uint32_t result;
	uint32_t sched = 0;
	
	uint32_t status = TaskEnterCritical();
  
  if (isSet)
	{
		flagGroup->flag |= flag;
	}
	else
	{
		flagGroup->flag &= ~flag;
	}
  
	for (node = waitList.headNode.postNode; node != (&waitList.headNode); node = node->postNode)
	{
		task_t * task = nodeParent(node, task_t, linkNode);
		uint32_t flags = task->eventFlag;
		
		result = FlagGroupCheckAndConsume(flagGroup, task->waitFlagType, &flags);
		if (result == ErrorNoError)
		{
			task->eventFlag = flags;
			EventWakeUpTask(&flagGroup->event, task, (void *)0, ErrorNoError);
			sched = 1;
		}
	}
	
	if (sched)
	{
		TaskSched();
	}
	
  TaskExitCritical(status);
}

uint32_t FlafGroupDestroy(flagGroup_t * flagGroup)
{
	uint32_t count;
	
	uint32_t status = TaskEnterCritical();
  
  count = EventRemoveAll(&flagGroup->event, (void *)0, ErrorDel);
  
  TaskExitCritical(status);
	
	if (count > 0)
	{
		TaskSched();
	}
	
	return count;
	
}

void FlagGroupGetInfo(flagGroup_t * flagGroup, flagGroupInfo_t * flagGroupInfo)
{
	uint32_t status = TaskEnterCritical();
  
  flagGroupInfo->flags = flagGroup->flag;
	flagGroupInfo->taskCount = EventWaitCount(&flagGroup->event);
  
  TaskExitCritical(status);
}

