#ifndef _TASK_H
#define _TASK_H


#define MYOS_TASK_STATE_RDY 					0
#define MYOS_TASK_STATE_DELAYED				(1<<1)
#define MYOS_TASK_STATE_SUSPENDED			(1<<2)
#define MYOS_TASK_STATE_DELETED				(1<<3)

#define MYOS_TASK_STATE_WAIT_MASK			(0xFF<<16)

struct _event_t;

typedef enum _error_t
{
	ErrorNoError = 0,
	ErrorTimeOut,
	ErrorResourceUnavaliable,
	ErrorDel,
	ErrorResourceFull,
}error_t;

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
	
	struct _event_t * waitEvent;
	void * eventMsg;
	uint32_t waitEventResult;
	
	uint32_t waitFlagType;
	uint32_t eventFlag;
	
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


void TaskSuspend (task_t * task);
void TaskWakeUp (task_t * task);
void TaskInit (task_t * task, void (*entry)(void *), void * param, taskStack_t * stack, uint32_t prio);
void TaskSetCleanCallFunc(task_t * task, void (*clean)(void * param), void * param);
void TaskForceDelete(task_t * task);
void TaskRequestDelete(task_t * task);
uint8_t TaskIsRequestDeleted(void);
void TaskDeleteSelf(void);
void TaskGetInfo(task_t * task, taskInfo_t * taskInfo);


#endif
