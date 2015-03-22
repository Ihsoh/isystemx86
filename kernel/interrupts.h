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
interrupts_init(void);

int32
get_unimpl_intn(void);

#endif
