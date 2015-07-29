#ifndef	_ISYS_SYSTEM_H_
#define	_ISYS_SYSTEM_H_

#include "types.h"

extern
VOIDPTR
ISYSAllocMemory(IN UINT32 size);

extern
VOID
ISYSFreeMemory(IN VOIDPTR mem);

extern
VOID
ISYSExit(IN INT32 code);

extern
BOOL
ISYSGetDateTime(OUT ISYSDateTimePtr dateTime);

extern
BOOL
ISYSRunInBackground(VOID);

extern
INT32
ISYSExecute(IN BYTEPTR path, IN BYTEPTR param);

extern
INT32
ISYSWait(IN INT32 tid);

#endif
