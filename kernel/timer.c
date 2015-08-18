/**
	@File:			timer.c
	@Author:		Ihsoh
	@Date:			2015-08-19
	@Description:
		定时器。
*/

#include "timer.h"
#include "types.h"

static uint32 _ticks;

/**
	@Function:		timer_init
	@Access:		Public
	@Description:
		初始化定时器。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
timer_init(void)
{
	_ticks = 0;
	return TRUE;
}

/**
	@Function:		timer_inc_ticks
	@Access:		Public
	@Description:
		滴答数递增1。
	@Parameters:
	@Return:
*/
void
timer_inc_ticks(void)
{
	_ticks++;
}

/**
	@Function:		timer_get_ticks
	@Access:		Public
	@Description:
		获取从定时器初始化以来的滴答数。
	@Parameters:
	@Return:
		uint32
			从定时器初始化以来的滴答数。
*/
uint32
timer_get_ticks(void)
{
	return _ticks;
}
