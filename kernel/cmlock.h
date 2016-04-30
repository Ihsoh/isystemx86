/**
	@File:			cmlock.h
	@Author:		Ihsoh
	@Date:			2015-04-21
	@Description:
		Common lock。
*/

#ifndef	_CMLOCK_H_
#define	_CMLOCK_H_

extern
void
KnlEnableCommonLock(void);

extern
void
KnlDisableCommonLock(void);

extern
void
_KnlCommonLock(void);

extern
void
_KnlCommonUnlock(	IN const int8 * file,
					IN uint32 line);

extern
void
_KnlCommonUnlockWithoutSti(	IN const int8 * file,
							IN uint32 line);

#define COMMON_LOCK()				_KnlCommonLock();
#define COMMON_UNLOCK()				_KnlCommonUnlock(__FILE__, __LINE__);
#define COMMON_UNLOCK_WITHOUT_STI()	_KnlCommonUnlockWithoutSti(__FILE__, __LINE__);

#endif
