//Filename:		gui.h
//Author:		Ihsoh
//Date:			2015-10-05
//Descriptor:	GUI

#ifndef	_GUI_H_
#define	_GUI_H_

#include "types.h"
#include "image.h"

#define	CONTROL_BUTTON		0x00000001
#define	CONTROL_LABEL		0x00000002
#define	CONTROL_LIST		0x00000003
#define	CONTROL_PROGRESS	0x00000004
#define	CONTROL_SCROLL		0x00000005

#include "../../kernel/syscall/syscall.h"

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

#include "../../kernel/window/event.h"

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
uint32
ILGGetKeyWait(IN int32 wid);

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

extern
BOOL
ILGGetMessage(	IN int32 wid,
				OUT int32 * cid,
				OUT uint32 * type,
				OUT void ** data);

extern
BOOL
ILGSetText(	IN int32 wid,
			IN int32 cid,
			IN CASCTEXT text);

extern
BOOL
ILGGetText(	IN int32 wid,
			IN int32 cid,
			OUT ASCTEXT text,
			IN uint32 bufsz);

extern
BOOL
ILGUpdate(IN int32 wid);

extern
BOOL
ILGRenderTextBuffer(IN int32 wid,
					IN uint8 * txtbuf,
					IN uint32 row,
					IN uint32 column,
					IN uint32 curx,
					IN uint32 cury);

extern
void
ILGFreeMsgData(IN void * data);

extern
BOOL
ILGEnableControl(	IN int32 wid,
					IN int32 cid);

extern
BOOL
ILGDisableControl(	IN int32 wid,
					IN int32 cid);

extern
BOOL
ILGGetWidth(IN int32 wid,
			IN int32 cid,
			OUT uint32 * width);

extern
BOOL
ILGGetHeight(	IN int32 wid,
				IN int32 cid,
				OUT uint32 * height);

extern
BOOL
ILGNewButton(	IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN CASCTEXT text,
				OUT int32 * cid);

extern
BOOL
ILGNewLabel(IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN CASCTEXT text,
			OUT int32 * cid);

extern
BOOL
ILGNewList(	IN int32 wid,
			IN uint32 count,
			IN int32 x,
			IN int32 y,
			IN CASCTEXT text,
			OUT int32 * cid);

extern
BOOL
ILGSetListText(	IN int32 wid,
				IN int32 cid,
				IN uint32 index,
				IN CASCTEXT text);

extern
BOOL
ILGEnableListItem(	IN int32 wid,
					IN int32 cid,
					IN uint32 index);

extern
BOOL
ILGDisableListItem(	IN int32 wid,
					IN int32 cid,
					IN uint32 index);

extern
BOOL
ILGNewEdit(	IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN uint32 row,
			IN uint32 column,
			IN CASCTEXT text,
			IN uint32 style,
			OUT int32 * cid);

extern
BOOL
ILGNewFileList(	IN int32 wid,
				IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 vscroll_width,
				OUT int32 * cid);

#endif
