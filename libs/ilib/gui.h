//Filename:		gui.h
//Author:		Ihsoh
//Date:			2015-10-05
//Descriptor:	GUI

#ifndef	_GUI_H_
#define	_GUI_H_

#include "types.h"
#include "image.h"

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
#define	SCALL_GUI_SET_WINDOW_TITLE		0x0000000a
#define	SCALL_GUI_GET_WINDOW_TITLE		0x0000000b
#define	SCALL_GUI_HAS_KEY				0x0000000c
#define	SCALL_GUI_GET_KEY				0x0000000d
#define	SCALL_GUI_GET_MOUSE				0x0000000e
#define	SCALL_GUI_FOCUSED				0x0000000f
#define	SCALL_GUI_FOCUS					0x00000010
#define	SCALL_GUI_SET_PIXEL				0x00000011
#define	SCALL_GUI_GET_PIXEL				0x00000012
#define	SCALL_GUI_DRAW_RECT				0x00000013
#define	SCALL_GUI_DRAW_IMAGE			0x00000014
#define	SCALL_GUI_DRAW_TEXT				0x00000015
#define	SCALL_GUI_DRAW_LINE				0x00000016

#define	WINDOW_STYLE_NONE				0x00000000
#define	WINDOW_STYLE_CLOSE				0x00000001
#define	WINDOW_STYLE_MINIMIZE			0x00000002
#define	WINDOW_STYLE_MAXIMIZE			0x00000004
#define	WINDOW_STYLE_NO_TITLE			0x00000008
#define	WINDOW_STYLE_NO_WMGR			0x00000010

#define	WINDOW_STATE_SHOW		1
#define	WINDOW_STATE_HIDDEN		2
#define	WINDOW_STATE_CLOSED		3

#define	MOUSE_BUTTON_NONE	0x00000000
#define	MOUSE_BUTTON_LEFT	0x00000001
#define	MOUSE_BUTTON_RIGHT	0x00000002
#define	MOUSE_BUTTON_MIDDLE	0x00000004

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

extern
BOOL
ILGHasKey(	IN int32 wid,
			OUT uint32 * key);

extern
BOOL
ILGGetKey(	IN int32 wid,
			OUT uint32 * key);

extern
BOOL
ILGGetMouse(IN int32 wid,
			OUT int32 * x,
			OUT int32 * y,
			OUT uint32 * button);

extern
BOOL
ILGFocused(IN int32 wid);

extern
BOOL
ILGFocus(IN int32 wid);

extern
BOOL
ILGSetPixel(IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN uint32 pixel);

extern
BOOL
ILGGetPixel(IN int32 wid,
			IN int32 x,
			IN int32 y,
			OUT uint32 * pixel);

extern
BOOL
ILGDrawRect(IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN int32 width,
			IN int32 height,
			IN uint32 pixel);

extern
BOOL
ILGDrawImage(	IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN int32 width,
				IN int32 height,
				IN ImagePtr image);

extern
BOOL
ILGDrawLine(IN int32 wid,
			IN int32 startx,
			IN int32 starty,
			IN int32 endx,
			IN int32 endy,
			IN uint32 color);

#endif
