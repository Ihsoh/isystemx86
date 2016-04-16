/**
	@File:			fs.h
	@Author:		Ihsoh
	@Date:			2014-7-29
	@Description:
*/

#ifndef	_SYSCALL_FS_H_
#define	_SYSCALL_FS_H_

#include "syscall.h"

#include "../types.h"
#include "../sparams.h"

extern
void
_ScFsProcess(	IN uint32 func,
				IN uint32 base,
				IN OUT struct SParams * sparams);

extern
void
ScFsUnlockFileSystem(void);

#endif
