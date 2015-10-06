/**
	@File:			gui.h
	@Author:		Ihsoh
	@Date:			2015-10-04
	@Description:
		GUI的系统调用。
*/

#ifndef	_SYSCALL_GUI_H_
#define	_SYSCALL_GUI_H_

#include "syscall.h"

#include "../types.h"
#include "../sparams.h"

#include "../window/control.h"

extern
void
system_call_gui(IN uint32 func,
				IN uint32 base,
				IN OUT struct SParams * sparams);

#endif
