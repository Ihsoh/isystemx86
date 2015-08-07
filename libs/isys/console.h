#ifndef	_ISYS_CONSOLE_H_
#define	_ISYS_CONSOLE_H_

#include "types.h"

#define	ISYS_TO_STDOUT		0
#define	ISYS_TO_STDERR		1

extern
BOOL
ISYSPrintChar(	IN UINT32 chr,
				IN UINT32 to);

extern
BOOL
ISYSPrintCharWithProperty(	IN UINT32 chr,
							IN UINT32 p,
							IN UINT32 to);

extern
BOOL
ISYSPrintString(IN BYTEPTR str,
				IN UINT32 to);

extern
BOOL
ISYSPrintStringWithProperty(IN BYTEPTR str,
							IN UINT32 p,
							IN UINT32 to);

extern
BOOL
ISYSClearConsole(VOID);

extern
BOOL
ISYSSetCaretPosition(	IN UINT32 x,
						IN UINT32 y);

extern
BOOL
ISYSGetCaretPosition(	OUT UINT32PTR x,
						OUT UINT32PTR y);

extern
BOOL
ISYSLockCaret(VOID);

extern
BOOL
ISYSUnlockCaret(VOID);

extern
BOOL
ISYSWriteConsoleBuffer(	IN BYTEPTR buffer,
						IN UINT32 size);

extern
BOOL
ISYSGetConsoleSize(	OUT UINT32PTR row,
					OUT UINT32PTR column);

extern
VOID
ISYSHideConsoleClock(VOID);

extern
VOID
ISYSShowConsoleClock(VOID);

#endif
