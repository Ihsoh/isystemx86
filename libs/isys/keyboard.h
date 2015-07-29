#ifndef	_ISYS_KEYBOARD_H_
#define	_ISYS_KEYBOARD_H_

#include "types.h"

#define	ISYS_CKEY_STATUS_CTRL	0x00000001
#define	ISYS_CKEY_STATUS_SHIFT	0x00000002
#define	ISYS_CKEY_STATUS_ALT	0x00000004

extern
UINT32
ISYSGetChar(VOID);

extern
UINT32
ISYSGetString(OUT BYTEPTR str);

extern
BOOL
ISYSHasChar(VOID);

extern
UINT32
ISYSGetControlKeyStatus(VOID);

#endif
