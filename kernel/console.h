/**
	@File:			console.h
	@Author:		Ihsoh
	@Date:			2014-2-1
	@Description:
*/

#ifndef	_CONSOLE_H_
#define	_CONSOLE_H_

#include "types.h"
#include "tasks.h"

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

extern
void
get_current_path(OUT int8 * path);

DEFINE_LOCK_EXTERN(console)

#endif
