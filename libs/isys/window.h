#ifndef	_ISYS_WINDOW_H_
#define	_ISYS_WINDOW_H_

#include "types.h"

#define	ISYS_WINDOW_STYLE_HIDDEN_BUTTON		0x00000001
#define	ISYS_WINDOW_STYLE_CLOSE_BUTTON		0x00000002

typedef UINT32 WID;

extern
WID
ISYSCreateWindow(	IN UINT32 width,
					IN UINT32 height,
					IN UINT32 bgColor,
					IN UINT32 style,
					IN BYTEPTR title);

extern
BOOL
ISYSShowWindow(IN WID wid);

extern
BOOL
ISYSHideWindow(IN WID wid);

extern
BOOL
ISYSDestroyWindow(IN WID wid);

extern
BOOL
ISYSSetPixelToWindow(	IN WID wid,
						IN UINT32 x,
						IN UINT32 y,
						IN UINT32 pixel);

extern
BOOL
ISYSGetPixelFromWindow(	IN WID wid,
						IN UINT32 x,
						IN UINT32 y,
						OUT UINT32PTR pixel);

extern
BOOL
ISYSDrawRawBitmapToWindow(	IN WID wid,
							IN UINT32 x,
							IN UINT32 y,
							IN UINT32 width,
							IN UINT32 height,
							IN BYTEPTR bitmap);

extern
BOOL
ISYSDrawTextToWindow(	IN WID wid,
						IN UINT32 x,
						IN UINT32 y,
						IN BYTEPTR text);

extern
BOOL
ISYSIsWindowTop(IN WID wid);

extern
BOOL
ISYSGetMousePositionInWindow(	IN WID wid,
								OUT UINT32PTR x,
								OUT UINT32PTR y);

extern
UINT32
ISYSGetKeyInWindow(IN WID wid);

#endif
