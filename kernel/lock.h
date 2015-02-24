/**
	@File:			lock.h
	@Author:		Ihsoh
	@Date:			2014-12-03
	@Description:
*/

#ifndef	_LOCK_H_
#define	_LOCK_H_

#include "types.h"
#include "screen.h"
#include "kstring.h"
#include "log.h"

//在*.c文件里定义锁的相关内容
#define	DEFINE_LOCK_IMPL(name)	\
	static int32 name##_lock_state = 0;	\
	void enable_##name##_lock(void)	\
	{	\
		name##_lock_state = 1;	\
	}	\
	void disable_##name##_lock(void)	\
	{	\
		name##_lock_state = 0;	\
	}	\
	static void _lock(void)	\
	{	\
		if(name##_lock_state)	\
		{	\
			lock_level++;	\
			asm volatile ("cli");	\
		}	\
	}	\
	static void _unlock(const int8 * file, uint32 line)	\
	{	\
		if(name##_lock_state)	\
		{	\
			if(lock_level == 0)	\
			{	\
				int8 buffer[1024];	\
				sprintf_s(	buffer,	\
							1024,	\
							"Invalid unlock() calling, because lock_level already is 0. File: %s, Line: %d.\n",	\
							file,	\
							line);	\
				print_str_p(buffer, CC_RED);	\
				log(LOG_ERROR, buffer);	\
			}	\
			else if(--lock_level == 0)	\
			{	\
				asm volatile ("sti");	\
			}	\
		}	\
	}

#define lock()		_lock();
#define unlock()	_unlock(__FILE__, __LINE__);

//在*.h文件里定义锁的相关内容
#define	DEFINE_LOCK_EXTERN(name)	\
	extern void enable_##name##_lock(void);	\
	extern void disable_##name##_lock(void);

extern int32 lock_level;

#endif
