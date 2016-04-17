/**
	@File:			dbgout.h
	@Author:		Ihsoh
	@Date:			2016-01-01
	@Description:
		调试输出窗体。
*/

#ifndef	_WINDOWS_DBGOUT_H_
#define	_WINDOWS_DBGOUT_H_

#include "../types.h"

extern
void
WinDbgoutInit(void);

extern
void
WinDbgoutShow(void);

extern
void
WinDbgoutOutput(IN CASCTEXT text);

#endif
