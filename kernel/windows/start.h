/**
	@File:			start.h
	@Author:		Ihsoh
	@Date:			2015-09-20
	@Description:
		任开始菜单。
*/

#ifndef	_WINDOWS_START_H_
#define	_WINDOWS_START_H_

#include "../types.h"
#include "../window.h"

extern
BOOL
start_window_init(void);

extern
BOOL
start_window_show(	IN int32 x,
					IN int32 taskbar_y);

extern
BOOL
start_window_hide(void);

#endif
