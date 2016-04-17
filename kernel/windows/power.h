/**
	@File:			power.h
	@Author:		Ihsoh
	@Date:			2015-09-20
	@Description:
		电源管理窗体。
*/

#ifndef	_WINDOWS_POWER_H_
#define	_WINDOWS_POWER_H_

#include "../types.h"
#include "../window.h"

extern
BOOL
WinPwrInit(void);

extern
BOOL
WinPwrShow(void);

extern
BOOL
WinPwrHide(void);

#endif
