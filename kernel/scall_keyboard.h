/**
	@File:			scall_keyboard.h
	@Author:		Ihsoh
	@Date:			2014-7-29
	@Description:
		键盘的系统调用处理程序。
*/

#ifndef	_SCALL_KEYBOARD_
#define	_SCALL_KEYBOARD_

#include "types.h"
#include "sparams.h"

#define	SCALL_GET_CHAR			0
#define	SCALL_GET_STR_N			1
#define	SCALL_GET_SHIFT			2
#define	SCALL_GET_CONTROL		3
#define	SCALL_GET_ALT			4
#define	SCALL_GET_KEY_STATUS	5
#define	SCALL_HAS_KEY			6

extern
void system_call_keyboard(	IN uint32 func,
							IN uint32 base,
							IN OUT struct SParams * sparams);

#endif
