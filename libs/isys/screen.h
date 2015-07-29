#ifndef	_ISYS_SCREEN_H_
#define	_ISYS_SCREEN_H_

#include "types.h"

extern
BOOL
ISYSIsGUIValid(VOID);

extern
BOOL
ISYSGetScreenSize(	OUT UINT32PTR width,
					OUT UINT32PTR height);

extern
BOOL
ISYSSetPixelToScreen(	IN UINT32 x,
						IN UINT32 y,
						IN UINT32 pixel);

extern
BOOL
ISYSGetPixelFromScreen(	IN UINT32 x,
						IN UINT32 y,
						OUT UINT32PTR pixel);

extern
BOOL
ISYSDrawRawBitmapToScreen(	IN UINT32 x,
							IN UINT32 y,
							IN UINT32 width,
							IN UINT32 height,
							IN BYTEPTR bitmap);

#endif
