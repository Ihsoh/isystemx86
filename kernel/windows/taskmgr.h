/**
	@File:			taskmgr.h
	@Author:		Ihsoh
	@Date:			2016-01-09
	@Description:
		任务管理器窗口。
*/

#ifndef	_WINDOWS_TASKMGR_H_
#define	_WINDOWS_TASKMGR_H_

#include "../types.h"

extern
void
taskmgr_window_init(void);

extern
void
taskmgr_window_show(void);

extern
void
taskmgr_window_hide(void);

extern
void
taskmgr_window_update(IN uint32 offset);

#endif
