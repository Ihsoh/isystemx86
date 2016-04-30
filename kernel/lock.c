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

static BOOL _common_lock_state = FALSE;

void
KnlEnableCommonLock(void)
{
	_common_lock_state = TRUE;
}

void
KnlDisableCommonLock(void)
{
	_common_lock_state = FALSE;
}

void
_KnlCommonLock(void)
{
	if(_common_lock_state)
	{
		lock_level++;
		asm volatile ("cli");
	}
}

void
_KnlCommonUnlock(	IN const int8 * file,
					IN uint32 line)
{
	if(_common_lock_state)
		if(lock_level == 0)
		{
			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"Invalid _KnlCommonUnlock() calling, because lock_level already is 0. File: %s, Line: %d.\n",
						file,
						line);
			ScrPrintStringWithProperty(buffer, CC_RED);
			Log(LOG_ERROR, buffer);
		}
		else if(--lock_level == 0)
		{
			asm volatile ("sti");
		}
}

void
_KnlCommonUnlockWithoutSti(	IN const int8 * file,
							IN uint32 line)
{
	if(_common_lock_state)
		if(lock_level == 0)
		{
			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"Invalid _KnlCommonUnlockWithoutSti() calling, because lock_level already is 0. File: %s, Line: %d.\n",
						file,
						line);
			ScrPrintStringWithProperty(buffer, CC_RED);
			Log(LOG_ERROR, buffer);
		}
		else if(--lock_level == 0)
		{
			;
		}
}
