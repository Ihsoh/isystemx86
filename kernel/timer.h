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

#define	MAX_TIMER_COUNT		256

typedef void (* TimerEvent)(void);

typedef struct
{
	uint32		interval;		// 引发事件的间隔时间，单位为：ms。
	TimerEvent	event;			// 时间函数。
	BOOL		enabled;		// 是否可用。
	uint32		ticks;			// 滴答。滴答为0时引发事件，并通过interval重置该值。
} Timer, * TimerPtr;

extern
BOOL
timer_init(void);

extern
void
timer_inc_ticks(void);

extern
uint32
timer_get_ticks(void);

extern
TimerPtr
timer_new(	IN uint32 interval,
			IN TimerEvent event);

extern
BOOL
timer_delete(IN TimerPtr timer);

extern
BOOL
timer_set_interval(	IN TimerPtr timer,
					IN uint32 interval);

extern
BOOL
timer_start(IN TimerPtr timer);

extern
BOOL
timer_stop(IN TimerPtr timer);

extern
uint32
timer_count(void);

extern
void
timer_clear(void);

#endif
