#ifndef _FLAG_H
#define _FLAG_H


#define FLAGGROUP_CLEAR			(0x00 << 0)
#define FLAGGROUP_SET				(0x01 << 0)
#define FLAGGROUP_ANY				(0x00 << 1)
#define FLAGGROUP_ALL				(0x01 << 1)

#define FLAGGROUP_SET_ALL			(FLAGGROUP_SET | FLAGGROUP_ALL)
#define FLAGGROUP_SET_ANY			(FLAGGROUP_SET | FLAGGROUP_ANY)
#define FLAGGROUP_CLEAR_ALL		(FLAGGROUP_CLEAR | FLAGGROUP_ALL)
#define FLAGGROUP_CLEAR_ANY		(FLAGGROUP_CLEAR | FLAGGROUP_ANY)

#define FLAGGROUP_CONSUME			(0x01 << 7)

typedef struct _flagGroup_t
{
	event_t event;
	uint32_t flag;
}flagGroup_t;


typedef struct _flagGroupInfo_t
{
	uint32_t flags;
	uint32_t taskCount;
}flagGroupInfo_t;

void FlagGroupInit(flagGroup_t * flagGroup, uint32_t flags);
uint32_t FlagGroupWait(flagGroup_t * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag, uint32_t waitTicks);
uint32_t FlagGroupNoWaitGet(flagGroup_t * flagGroup, uint32_t waitType, uint32_t requestFlag, uint32_t * resultFlag);
void FlagGroupNotify(flagGroup_t * flagGroup, uint8_t isSet, uint32_t flag);
uint32_t FlafGroupDestroy(flagGroup_t * flagGroup);
void FlagGroupGetInfo(flagGroup_t * flagGroup, flagGroupInfo_t * flagGroupInfo);


#endif 
