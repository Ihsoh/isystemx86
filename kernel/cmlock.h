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
enable_common_lock(void);

extern
void
disable_common_lock(void);

extern
void
_common_lock(void);

extern
void
_common_unlock(	IN const int8 * file,
				IN uint32 line);

extern
void
_common_unlock_without_sti(	IN const int8 * file,
							IN uint32 line);

#define common_lock()				_common_lock();
#define common_unlock()				_common_unlock(__FILE__, __LINE__);
#define common_unlock_without_sti()	_common_unlock_without_sti(__FILE__, __LINE__);

#endif
