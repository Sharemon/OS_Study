#ifndef _MYOS_H
#define _MYOS_H

#include <stdint.h>

typedef uint32_t TaskStack_t;

typedef struct _Task_t
{
	TaskStack_t * stackPtr;
} Task_t;

#endif
