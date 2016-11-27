/**
	@File:			sb16.h
	@Author:		Ihsoh
	@Date:			2016-11-19
	@Description:
*/

#ifndef	_SB_SB16_H_
#define	_SB_SB16_H_

#include "../types.h"

extern
void
SB16Interrupt(
	IN uint32 peripheral,
	IN uint32 irq);

extern
BOOL
SB16Playback(
	IN uint8 * data,
	IN uint32 len,
	IN uint32 frequency,
	IN BOOL bits16,
	IN BOOL stereo,
	IN BOOL auto_init);

extern
BOOL
SB16Stop(void);

extern
BOOL
SB16Pause(void);

extern
BOOL
SB16Resume(void);

extern
void
SB16Init(void);

#endif
