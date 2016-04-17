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
ConEnterConsole(void);

extern
void
ConUpdateClock(void);

extern
int32
ConGetCurrentApplicationTid(void);

extern
void
ConSetCurrentApplicationTid(int32 tid);

extern
void
ConSetClock(int32 enable);

extern
void
ConGetCurrentDir(OUT int8 * path);

extern
BOOL
ConInit(void);

extern
void
ConExitBatchScript(void);

extern
int32
ConExecuteCommand(IN CASCTEXT cmd);

DEFINE_LOCK_EXTERN(console)

#endif
