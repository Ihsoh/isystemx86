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
	static void lock(void)	\
	{	\
		if(name##_lock_state)	\
		{	\
			lock_level++;	\
			asm volatile ("cli");	\
		}	\
	}	\
	static void unlock(void)	\
	{	\
		if(name##_lock_state)	\
		{	\
			if(--lock_level == 0)	\
			{	\
				asm volatile ("sti");	\
			}	\
		}	\
	}

//在*.h文件里定义锁的相关内容
#define	DEFINE_LOCK_EXTERN(name)	\
	extern void enable_##name##_lock(void);	\
	extern void disable_##name##_lock(void);

extern int32 lock_level;

#endif
