#include "myOS.h"

void MBoxInit(mBox_t * mbox, void ** msgBuffer, uint32_t maxCount)
{
	EventInit(&mbox->event, EventTypeMBox);
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;
	mbox->maxCount = maxCount;
	mbox->msgBuffer = msgBuffer;
}


uint32_t MBoxWait(mBox_t * mbox, void ** msg, uint32_t waitTicks)
{
	uint32_t status = TaskEnterCritical();
  
  if (mbox->count > 0)
	{
		--mbox->count;
		
		*msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		
		TaskExitCritical(status);
		return ErrorNoError;
		
	}
	else 
	{	
		EventWait(&mbox->event, curTask, (void *)0, EventTypeMBox, waitTicks);
		
		TaskExitCritical(status);
		
		TaskSched();
		
		*msg = curTask->eventMsg;
		
		return curTask->waitEventResult;
	}
}


uint32_t MBoxNoWaitGet(mBox_t * mbox, void ** msg)
{
	uint32_t status = TaskEnterCritical();
  
  if (mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		
		TaskExitCritical(status);
		return ErrorNoError;
		
	}
	else
	{
		TaskExitCritical(status);
		
		*msg = (void *)0;
		return ErrorResourceUnavaliable;
	}
}


uint32_t MBoxNotify(mBox_t * mbox, void * msg, uint32_t notifyOption)
{
	uint32_t status = TaskEnterCritical();
  
  if (EventWaitCount(&mbox->event) > 0)
	{
		task_t * task = EventWakeUp(&mbox->event, msg, ErrorNoError);
		if (task ->prio < curTask->prio)
		{
			TaskSched();
		}
	}
	else 
	{
		if (mbox->count >= mbox->maxCount)
		{
			TaskExitCritical(status);
			return ErrorResourceFull;
		}
		
		if (notifyOption & MBoxSendFront)
		{
			if (mbox->read <= 0)
			{
				mbox->read = mbox->maxCount-1;
			}
			else
			{
				--mbox->read;
			}
			
			mbox->msgBuffer[mbox->read] = msg;
		}
		else
		{
			mbox->msgBuffer[mbox->write++] = msg;
			if(mbox->write >= mbox->maxCount)
			{
				mbox->write = 0;
			}
		}
		
		mbox->count++;
		
	}
	
	TaskExitCritical(status);
		
	return ErrorNoError;
}


void MBoxEmpty(mBox_t * mbox)
{
	uint32_t status = TaskEnterCritical();
  
  if (mbox->count != 0)
	{
		mbox->read = 0;
		mbox->write = 0;
		mbox->count = 0;
	}
  
  TaskExitCritical(status);
}

uint32_t MBoxDestroy(mBox_t * mbox)
{
	int count = 0;
	
	uint32_t status = TaskEnterCritical();
  
  count = EventRemoveAll(&mbox->event, (void *)0, ErrorDel);
  
  TaskExitCritical(status);
	
	if (count > 0)
	{
		TaskSched();
	}
	
	return count;
	
}

void MBoxGetInfo(mBox_t * mbox, mBoxtInfo_t * mboxInfo)
{
	uint32_t status = TaskEnterCritical();
  
  mboxInfo->count = mbox->count;
	mboxInfo->maxCount = mbox->maxCount;
	mboxInfo->taskCount = EventWaitCount(&mbox->event);
  
  TaskExitCritical(status);
}

