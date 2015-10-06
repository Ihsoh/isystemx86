/**
	@File:			mouse.h
	@Author:		Ihsoh
	@Date:			2014-11-14
	@Description:
*/

#ifndef	_SYSCALL_MOUSE_
#define	_SYSCALL_MOUSE_

#include "syscall.h"

#include "../types.h"
#include "../sparams.h"

extern
void
system_call_mouse(	IN uint32 func,
					IN uint32 base,
					IN OUT struct SParams * sparams);

#endif
