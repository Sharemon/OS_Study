#include "tLib.h"

void BitmapInit(bitmap_t * bitmap)
{
	bitmap->bitmap = 0;
}
void BitmapSet(bitmap_t * bitmap, uint32_t pos)
{
	bitmap->bitmap |= (0x01 << pos);
}
void BitmapClear(bitmap_t * bitmap, uint32_t pos)
{
	bitmap->bitmap &= ~(0x01 << pos);
}

uint32_t BitmapGetFirstSet(bitmap_t * bitmap)
{
	static const uint8_t quickFindTable[256] = 
	{
			/* 00 */ 0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 10 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 20 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 30 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 40 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 50 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 60 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 70 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 80 */ 7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 90 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* A0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* B0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* C0 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* D0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* E0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* F0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
	};
	
	if (bitmap->bitmap & 0xff)
	{
		return quickFindTable[bitmap->bitmap&0xff];
	}
	else if (bitmap->bitmap & 0xff00)
	{
		return quickFindTable[(bitmap->bitmap >>8 )&0xff] + 8;	// plus 8
	}
	else if (bitmap->bitmap & 0xff0000)
	{
		return quickFindTable[(bitmap->bitmap >>16 )&0xff] + 16;	// plus 16
	}
	else if (bitmap->bitmap & 0xff000000)
	{
		return quickFindTable[(bitmap->bitmap >>24 )&0xff] + 24;	// plus 24
	}
	
	return BitmapPosCount();
}



uint32_t BitmapPosCount(void)
{
	return 32;
}