#ifndef _EVENT_H
#define _EVENT_H


//#include "list.h"
//#include "task.h"

typedef enum _eventType_t {
	EventTypeUnknow,
	EventTypeSem,
	EventTypeMBox,
}eventType_t;

typedef struct _event_t {
	eventType_t type;
	list_t waitList;
}event_t;

void EventInit (event_t * event, eventType_t type);
void EventWait (event_t * event, task_t * task, void * msg, uint32_t state, uint32_t timeout);
task_t * EventWakeUp (event_t * event, void * msg, uint32_t result);
void EventRemoveTask (task_t *task, void * msg, uint32_t result);
uint32_t EventWaitCount (event_t * event);
uint32_t EventRemoveAll (event_t * event, void * msg, uint32_t result);


#endif
