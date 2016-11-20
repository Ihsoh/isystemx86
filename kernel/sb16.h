/**
	@File:			sb16.h
	@Author:		Ihsoh
	@Date:			2016-11-19
	@Description:
*/

#ifndef	_SB16_H_
#define	_SB16_H_

#include "types.h"

extern
void
SB16Interrupt(
	IN uint32 peripheral,
	IN uint32 irq);

extern
BOOL
SB16Playback(
	IN uint32 n,
	IN uint8 * data,
	IN uint32 len,
	IN uint32 frequency,
	IN BOOL bits16,
	IN BOOL stereo);

extern
void
SB16Init(void);

#endif
