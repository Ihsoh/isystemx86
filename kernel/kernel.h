/**
	@File:			kernel.h
	@Author:		Ihsoh
	@Date:			2014-7-26
	@Description:
*/

#ifndef	_KERNEL_H_
#define	_KERNEL_H_

#include "syscall/syscall.h"
#include "lock.h"

extern
BOOL
KnlIsCurrentlyKernelTask(void);

extern
int32
KnlGetCurrentTaskId(void);

extern
void
KnlGetMousePosition(OUT int32 * x,
					OUT int32 * y);

extern
BOOL
KnlIsMouseLeftButtonDown(void);

extern
BOOL
KnlIsMouseRightButtonDown(void);

extern
void
KnlShutdownSystem(void);

extern
void
KnlRebootSystem(void);

extern
BOOL
KnlResetSystemCall(IN int32 tid);

extern
void
KnlFreeSystemCall(IN int32 tid);

extern
BOOL
KnlGetIdeSignal(IN BOOL primary);

#define GET_ALL_IDE_SIGNAL() (KnlGetIdeSignal(TRUE) || KnlGetIdeSignal(FALSE))

extern
void
KnlClearIdeSignal(IN BOOL primary);

#define CLEAR_ALL_IDE_SIGNAL() { KnlClearIdeSignal(TRUE); KnlClearIdeSignal(FALSE); }

extern
void
KnlResetIdeSignal(IN BOOL primary);

#define RESET_ALL_IDE_SIGNAL() { KnlResetIdeSignal(TRUE); KnlResetIdeSignal(FALSE); }

DEFINE_LOCK_EXTERN(kernel)

#endif
