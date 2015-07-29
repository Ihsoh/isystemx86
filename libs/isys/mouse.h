#ifndef	_ISYS_MOUSE_H_
#define	_ISYS_MOUSE_H_

#include "types.h"

#define	ISYS_MBUTTON_STATUS_LEFT	0x00000001
#define	ISYS_MBUTTON_STATUS_RIGHT	0x00000002
#define	ISYS_MBUTTON_STATUS_MIDDLE	0x00000004

extern
BOOL
ISYSGetMousePosition(	OUT UINT32PTR x,
						OUT UINT32PTR y);

extern
UINT32
ISYSGetMouseButtonStatus(VOID);

#endif
