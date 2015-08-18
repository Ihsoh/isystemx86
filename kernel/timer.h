/**
	@File:			timer.h
	@Author:		Ihsoh
	@Date:			2015-08-19
	@Description:
		定时器。
*/

#ifndef	_TIMER_H_
#define	_TIMER_H_

#include "types.h"

BOOL
timer_init(void);

void
timer_inc_ticks(void);

uint32
timer_get_ticks(void);

#endif
