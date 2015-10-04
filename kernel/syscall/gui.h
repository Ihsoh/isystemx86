/**
	@File:			gui.h
	@Author:		Ihsoh
	@Date:			2015-10-04
	@Description:
		GUI的系统调用。
*/

#ifndef	_SYSCALL_GUI_H_
#define	_SYSCALL_GUI_H_

#include "../types.h"
#include "../sparams.h"
#include "../lock.h"

#define	SCALL_GUI_IS_VALID				0x00000001
#define	SCALL_GUI_CREATE_WINDOW			0x00000002
#define	SCALL_GUI_CLOSE_WINDOW			0x00000003
#define	SCALL_GUI_SET_WINDOW_STATE		0x00000004
#define	SCALL_GUI_GET_WINDOW_STATE		0x00000005
#define	SCALL_GUI_SET_WINDOW_POSITION	0x00000006
#define	SCALL_GUI_GET_WINDOW_POSITION	0x00000007
#define	SCALL_GUI_SET_WINDOW_SIZE		0x00000008
#define	SCALL_GUI_GET_WINDOW_SIZE		0x00000009
#define	SCALL_GUI_SET_WINDOW_TITLE		0x0000000A
#define	SCALL_GUI_GET_WINDOW_TITLE		0x0000000B
#define	SCALL_GUI_GET_MOUSE_STATUS		0x0000000C

extern
void
system_call_gui(IN uint32 func,
				IN uint32 base,
				IN OUT struct SParams * sparams);

#endif
