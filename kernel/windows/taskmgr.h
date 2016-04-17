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
WinTskmgrInit(void);

extern
void
WinTskmgrShow(void);

extern
void
WinTskmgrHide(void);

extern
void
WinTskmgrUpdate(IN uint32 offset);

#endif
