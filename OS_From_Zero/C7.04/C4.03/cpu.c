#include "myOS.h"
#include "ARMCM3.h"

void SysTick_Handler(void)
{
	TaskSystemTickHandler();
}

void SysTickInit(uint32_t n)
{
	// Init SysTick
	SysTick->LOAD = n * SystemCoreClock/1000 -1;
	NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS)-1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
									SysTick_CTRL_TICKINT_Msk |
									SysTick_CTRL_ENABLE_Msk;
}


