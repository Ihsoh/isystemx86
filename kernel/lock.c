/**
	@File:			lock.c
	@Author:		Ihsoh
	@Date:			2014-12-03
	@Description:
		提供锁的功能。
*/

#include "lock.h"
#include "types.h"

int32 lock_level = 0;
