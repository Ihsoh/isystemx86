/**
	@File:			scall_system.h
	@Author:		Ihsoh
	@Date:			2014-7-29
	@Description:
*/

#ifndef	_SCALL_SYSTEM_H_
#define	_SCALL_SYSTEM_H_

#include "types.h"
#include "sparams.h"
#include "lock.h"

#define	SCALL_EXIT			0
#define	SCALL_ALLOC_M		1
#define	SCALL_FREE_M		2
#define	SCALL_GET_PARAM		3
#define	SCALL_GET_DATETIME	4
#define	SCALL_SET_CLOCK		5

extern
void system_call_system(IN uint32 func,
						IN uint32 base,
						IN OUT struct SParams * sparams);

#endif
