//Filename:		gui.h
//Author:		Ihsoh
//Date:			2015-10-05
//Descriptor:	GUI

#ifndef	_GUI_H_
#define	_GUI_H_

#include "types.h"

#define SCALL_GUI			5

#define	SCALL_GUI_IS_VALID				0x00000001
#define	SCALL_GUI_CREATE_WINDOW			0x00000002
#define	SCALL_GUI_CLOSE_WINDOW			0x00000003
#define	SCALL_GUI_SET_WINDOW_STATE		0x00000004
#define	SCALL_GUI_GET_WINDOW_STATE		0x00000005
#define	SCALL_GUI_SET_WINDOW_POSITION	0x00000006
#define	SCALL_GUI_GET_WINDOW_POSITION	0x00000007
#define	SCALL_GUI_SET_WINDOW_SIZE		0x00000008
#define	SCALL_GUI_GET_WINDOW_SIZE		0x00000009
#define	SCALL_GUI_SET_WINDOW_TITLE		0x0000000A
#define	SCALL_GUI_GET_WINDOW_TITLE		0x0000000B
#define	SCALL_GUI_GET_MOUSE_STATUS		0x0000000C

#define	WINDOW_STYLE_NONE				0x00000000
#define	WINDOW_STYLE_CLOSE				0x00000001
#define	WINDOW_STYLE_MINIMIZE			0x00000002
#define	WINDOW_STYLE_MAXIMIZE			0x00000004
#define	WINDOW_STYLE_NO_TITLE			0x00000008
#define	WINDOW_STYLE_NO_WMGR			0x00000010

#define	WINDOW_STATE_SHOW		1
#define	WINDOW_STATE_HIDDEN		2
#define	WINDOW_STATE_CLOSED		3

extern
BOOL
ILGIsValid(void);

extern
int32
ILGCreateWindow(IN uint32		width,
				IN uint32		height,
				IN uint32		bgcolor,
				IN uint32		style,
				IN CASCTEXT		title);

extern
BOOL
ILGCloseWindow(IN int32 wid);

extern
BOOL
ILGSetWindowState(	IN int32 wid,
					IN uint32 state);

extern
BOOL
ILGGetWindowState(	IN int32 wid,
					IN uint32 * state);

extern
BOOL
ILGSetWindowPosition(	IN int32 wid,
						IN int32 x,
						IN int32 y);

extern
BOOL
ILGGetWindowPosition(	IN int32 wid,
						IN int32 * x,
						IN int32 * y);

extern
BOOL
ILGSetWindowSize(	IN int32 wid,
					IN uint32 width,
					IN uint32 height);

extern
BOOL
ILGGetWindowSize(	IN int32 wid,
					IN uint32 * width,
					IN uint32 * height);

extern
BOOL
ILGSetWindowTitle(	IN int32 wid,
					IN CASCTEXT title);

extern
BOOL
ILGGetWindowTitle(	IN int32 wid,
					IN ASCTEXT title);

#endif
