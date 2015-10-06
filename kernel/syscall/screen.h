/**
	@File:			screen.h
	@Author:		Ihsoh
	@Date:			2014-7-28
	@Description:
*/

#ifndef	_SYSCALL_SCREEN_
#define	_SYSCALL_SCREEN_

#include "syscall.h"

#include "../types.h"
#include "../sparams.h"

extern
void
system_call_screen(	IN uint32 func,
					IN uint32 base,
					IN OUT struct SParams * sparams);

#endif
