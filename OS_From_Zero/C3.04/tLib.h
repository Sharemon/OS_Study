#ifndef _TLIB_H
#define _TLIB_H

#include <stdint.h>

typedef struct
{
	uint32_t bitmap;
}bitmap_t;

void BitmapInit(bitmap_t * bitmap);
void BitmapSet(bitmap_t * bitmap, uint32_t pos);
void BitmapClear(bitmap_t * bitmap, uint32_t pos);

uint32_t BitmapGetFirstSet(bitmap_t * bitmap);
uint32_t BitmapPosCount(void);

#endif
