/**
	@File:			interrupts.h
	@Author:		Ihsoh
	@Date:			2015-03-21
	@Description:
*/

#ifndef	_INTERRUPTS_H_
#define	_INTERRUPTS_H_

#include "types.h"

BOOL
KnlInitInterrupts(void);

int32
KnlGetUnimplementedInterruptNo(void);

#endif
