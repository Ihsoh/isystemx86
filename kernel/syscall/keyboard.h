/**
	@File:			keyboard.h
	@Author:		Ihsoh
	@Date:			2014-7-29
	@Description:
		键盘的系统调用处理程序。
*/

#ifndef	_SYSCALL_KEYBOARD_
#define	_SYSCALL_KEYBOARD_

#include "syscall.h"

#include "../types.h"
#include "../sparams.h"

extern
void
system_call_keyboard(	IN uint32 func,
						IN uint32 base,
						IN OUT struct SParams * sparams);

#endif
