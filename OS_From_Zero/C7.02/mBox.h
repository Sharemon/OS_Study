#ifndef _MBOX_H
#define _MBOX_H

#define MBoxSendNormal	0x00
#define MBoxSendFront 	0x01


typedef struct _mBox_t
{
	event_t event;
	uint32_t count;
	uint32_t read;
	uint32_t write;
	uint32_t maxCount;
	void ** msgBuffer;
}mBox_t;


void MBoxInit(mBox_t * mBox, void ** msgBuffer, uint32_t maxCount);
uint32_t MBoxWait(mBox_t * mbox, void ** msg, uint32_t waitTicks);
uint32_t MBoxNoWaitGet(mBox_t * mbox, void ** msg);
uint32_t MBoxNotify(mBox_t * mbox, void * msg, uint32_t notifyOption);




#endif
