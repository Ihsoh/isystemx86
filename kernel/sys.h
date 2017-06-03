/**
	@File:			sys.h
	@Author:		Ihsoh
	@Date:			2017-6-3
	@Description:
		系统相关函数。
*/

#ifndef	_SYS_H_
#define	_SYS_H_

#include "types.h"
#include "time.h"

extern
BOOL
KnlGetCurrentDateTime(
	OUT DateTimePtr dt);

#endif
