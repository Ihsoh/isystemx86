/**
	@File:			knlstatic.h
	@Author:		Ihsoh
	@Date:			2015-05-12
	@Description:
		kernel.c中的static函数声明。
		该文件只会被kernel.c包含。
*/

#ifndef	_KNLSTATIC_H_
#define	_KNLSTATIC_H_

#include "types.h"

static
void
_KnlResetAllExceptions(void);

static
void
_KnlInitDiskVA(void);

static
void *
_KnlLibMalloc(IN uint32 num_bytes);

static
void *
_KnlLibCalloc(	IN uint32 n, 
				IN uint32 size);

static
void
_KnlLibFree(IN void * ptr);

static
BOOL
_KnlLibInitDSL(void);

static
BOOL
_KnlLibInitJson(void);

static
BOOL
_KnlLibInitPath(void);

static
BOOL
_KnlLibInitMemPool(void);

static
void
_KnlIrqAck(IN uint32 no);

static
void
_KnlInitInterrupt(void);

static
void
_KnlFpuErrorInterrupt(void);

static
void
_KnlFillTSS(OUT struct TSS * tss, 
			IN uint32 eip,
			IN uint32 esp);

static
void
_KnlInitTimer(void);

static
void
_KnlInitMouse(void);

static
void
_KnlInitKeyboard(void);

static
void
_KnlInitIde(void);

static
void
_KnlInitIde1(void);

static
void
_KnlInitFpu(void);

static
void
_KnlFreeSystemCallTSS(void);

static
void
_KnlFreeTaskTSS(void);

static
void
_KnlTimerInterrupt(void);

static
void
_KnlMouseInterrupt(void);

static
void
_KnlKeyboardInterrupt(void);

static
void
_KnlIdeInterrupt(void);

static
void
_KnlIde1Interrupt(void);

static
void
_KnlFpuInterrupt(void);

static
void
_KnlInitSystemCall(void);

static
void
_KnlProcessSystemCall(void);

static
void
_KnlSystemCallInterrupt(void);

static
void
_KnlKillTaskAndJumpToKernel(IN uint32 tid);

static
void
_KnlInitDividingByZeroException(void);

static
void
_KnlDividingByZeroExceptionInterrupt(void);

static
void
_KnlInitBoundCheckException(void);

static
void
_KnlBoundCheckExceptionInterrupt(void);

static
void
_KnlInitInvalidOpcodeException(void);

static
void
_KnlInvalidOpcodeExceptionInterrupt(void);

static
void
_KnlInitDoubleFaultException(void);

static
void
_KnlDoubleFaultExceptionInterrupt(void);

static
void
_KnlInitInvalidTSSException(void);

static
void
_KnlInvalidTSSExceptionInterrupt(void);

static
void
_KnlInitInvalidSegmentException(void);

static
void
_KnlInvalidSegmentExceptionInterrupt(void);

static
void
_KnlInitInvalidStackException(void);

static
void
_KnlInvalidStackExceptionInterrupt(void);

static
void
_KnlInitGlobalProtectionException(void);

static
void
_KnlGlobalProtectionExceptionInterrupt(void);

static
void
_KnlInitPageFaultException(void);

static
void
_KnlPageFaultExceptionInterrupt(void);

static
void
_KnlInitMathFaultException(void);

static
void
_KnlMathFaultExceptionInterrupt(void);

static
void
_KnlInitAlignCheckException(void);

static
void
_KnlAlignCheckExceptionInterrupt(void);

static
void
_KnlInitMachineCheckException(void);

static
void
_KnlMachineCheckExceptionInterrupt(void);

static
void
_KnlInitSIMDFloatPointException(void);

static
void
_KnlSIMDFloatPointExceptionInterrupt(void);

static
void
_KnlInitUnimplementedInterruptException(void);

static
void
_KnlSetUnimplementedInterrupt(IN uint32 int_num);

static
void
_KnlUnimplementedInterrupt(void);

static
void
_KnlEnterSystem(void);

static
void
_KnlLeaveSystem(void);

#endif
