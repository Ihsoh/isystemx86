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
dbgout_window_init(void);

extern
void
dbgout_window_show(void);

extern
void
dbgout_window_output(IN CASCTEXT text);

#endif
