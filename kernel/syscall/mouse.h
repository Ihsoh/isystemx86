/**
	@File:			mouse.h
	@Author:		Ihsoh
	@Date:			2014-11-14
	@Description:
*/

#ifndef	_SYSCALL_MOUSE_
#define	_SYSCALL_MOUSE_

#include "../types.h"
#include "../sparams.h"

#define	SCALL_GET_MPOSITION		0
#define	SCALL_GET_MLBSTATE		1
#define	SCALL_GET_MRBSTATE		2

extern
void
system_call_mouse(	IN uint32 func,
					IN uint32 base,
					IN OUT struct SParams * sparams);

#endif
