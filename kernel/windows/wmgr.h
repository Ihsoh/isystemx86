/**
	@File:			wmgr.h
	@Author:		Ihsoh
	@Date:			2015-08-15
	@Description:
		窗体管理器。
*/

#ifndef	_WINDOWS_WMGR_H_
#define	_WINDOWS_WMGR_H_

#include "../types.h"
#include "../window.h"

extern
BOOL
wmgr_window_init(void);

extern
BOOL
wmgr_window_show(	IN int32 x,
					IN int32 taskbar_y);

extern
BOOL
wmgr_window_hide(void);

extern
void
wmgr_window_update(IN uint32 offset);

#endif
		