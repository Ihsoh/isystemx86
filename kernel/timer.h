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
TmrInit(void);

extern
void
TmrDispatchTick(void);

extern
void
TmrIncTicks(void);

extern
uint32
TmrGetTicks(void);

extern
TimerPtr
TmrNew(	IN uint32 interval,
		IN TimerEvent event);

extern
BOOL
TmrDelete(IN TimerPtr timer);

extern
BOOL
TmrSetInterval(	OUT TimerPtr timer,
				IN uint32 interval);

extern
BOOL
TmrStart(OUT TimerPtr timer);

extern
BOOL
TmrStop(OUT TimerPtr timer);

extern
uint32
TmrGetCount(void);

extern
void
TmrClear(void);

extern
BOOL
TmrSetEvent(OUT TimerPtr timer,
			IN TimerEvent event);

#endif
