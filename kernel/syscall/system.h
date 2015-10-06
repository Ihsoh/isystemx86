/**
	@File:			system.h
	@Author:		Ihsoh
	@Date:			2014-7-29
	@Description:
*/

#ifndef	_SYSCALL_SYSTEM_H_
#define	_SYSCALL_SYSTEM_H_

#include "syscall.h"

#include "../types.h"
#include "../sparams.h"
#include "../lock.h"

extern
void
system_call_system(	IN uint32 func,
					IN uint32 base,
					IN OUT struct SParams * sparams);

#endif
