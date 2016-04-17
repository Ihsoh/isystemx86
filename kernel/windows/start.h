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
WinStartInit(void);

extern
BOOL
WinStartShow(	IN int32 x,
				IN int32 taskbar_y);

extern
BOOL
WinStartHide(void);

#endif
