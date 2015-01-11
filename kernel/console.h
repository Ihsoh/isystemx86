/**
	@File:			console.h
	@Author:		Ihsoh
	@Date:			2014-2-1
	@Description:
*/

#ifndef	_CONSOLE_H_
#define	_CONSOLE_H_

#include "types.h"

extern
void
console(void);

extern
void
console_clock(void);

extern
int32
get_wait_app_tid(void);

extern
void
set_wait_app_tid(int32 tid);

extern
void
set_clock(int32 enable);

#endif
