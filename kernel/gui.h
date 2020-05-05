/**
	@File:			gui.h
	@Author:		Ihsoh
	@Date:			2015-10-05
	@Description:
		用户模式GUI功能。
*/

#ifndef	_GUI_H_
#define	_GUI_H_

#include "types.h"
#include "lock.h"

#define GUI_MAX_MESSAGE_COUNT	16

typedef struct
{
	int32		wid;
	int32		cid;
	uint32		type;
	void *		data;			// 用户态的内存地址，为NULL无效。
	void *		phyaddr_data;	// 内核态的内存地址，为NULL无效。
} WindowMessage, * WindowMessagePtr;

extern
BOOL
GuiInit(void);

extern
BOOL
GuiIsEnabled(void);

extern
int32
GuiCreateWindow(IN int32		tid,
				IN uint32		width,
				IN uint32		height,
				IN uint32		bgcolor,
				IN uint32		style,
				IN CASCTEXT		title);

extern
BOOL
GuiClearMessages(	IN int32 tid,
					IN int32 wid);

extern
BOOL
GuiCloseWindow(	IN int32 tid,
				IN int32 wid);

extern
void
GuiCloseWindows(IN int32 tid);

extern
BOOL
GuiSetWindowStatus(	IN int32 tid,
					IN int32 wid,
					IN uint32 state);

extern
BOOL
GuiGetWindowStatus(	IN int32 tid,
					IN int32 wid,
					OUT uint32 * state);

extern
BOOL
GuiSetWindowPosition(	IN int32 tid,
						IN int32 wid,
						IN int32 x,
						IN int32 y);

extern
BOOL
GuiGetWindowPosition(	IN int32 tid,
						IN int32 wid,
						OUT int32 * x,
						OUT int32 * y);

extern
BOOL
GuiSetWindowSize(	IN int32 tid,
					IN int32 wid,
					IN uint32 width,
					IN uint32 height);

extern
BOOL
GuiGetWindowSize(	IN int32 tid,
					IN int32 wid,
					OUT uint32 * width,
					OUT uint32 * height);

extern
BOOL
GuiSetWindowTitle(	IN int32 tid,
					IN int32 wid,
					IN CASCTEXT title);

extern
BOOL
GuiGetWindowTitle(	IN int32 tid,
					IN int32 wid,
					OUT ASCTEXT title);

extern
BOOL
GuiHasKey(	IN int32 tid,
			IN int32 wid,
			OUT uint32 * key);

extern
BOOL
GuiGetKey(	IN int32 tid,
			IN int32 wid,
			OUT uint32 * key);

extern
BOOL
GuiGetMouse(	IN int32 tid,
				IN int32 wid,
				OUT int32 * x,
				OUT int32 * y,
				OUT uint32 * button);

extern
BOOL
GuiIsFocused(	IN int32 tid,
				IN int32 wid);

extern
BOOL
GuiFocus(	IN int32 tid,
			IN int32 wid);

extern
BOOL
GuiSetPixel(IN int32 tid,
			IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN uint32 pixel);

extern
BOOL
GuiGetPixel(IN int32 tid,
			IN int32 wid,
			IN int32 x,
			IN int32 y,
			OUT uint32 * pixel);

extern
BOOL
GuiDrawRect(IN int32 tid,
			IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN int32 width,
			IN int32 height,
			IN uint32 pixel);

extern
BOOL
GuiDrawImage(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN int32 width,
				IN int32 height,
				IN ImagePtr image);

extern
BOOL
GuiDrawText(IN int32 tid,
			IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN CASCTEXT text,
			IN uint32 color);

extern
BOOL
GuiDrawLine(IN int32 tid,
			IN int32 wid,
			IN int32 startx,
			IN int32 starty,
			IN int32 endx,
			IN int32 endy,
			IN uint32 color);

extern
BOOL
GuiPushMessage(	IN int32 tid,
				IN int32 wid,
				IN int32 cid,
				IN uint32 type,
				IN void * data,
				IN void * phyaddr_data);

extern
BOOL
GuiPopMessage(	IN int32 tid,
				IN int32 wid,
				OUT int32 * cid,
				OUT uint32 * type,
				OUT void ** data,
				OUT void ** phyaddr_data);

extern
BOOL
GuiSetText(	IN int32 tid,
			IN int32 wid,
			IN int32 id,
			IN CASCTEXT text);

extern
BOOL
GuiGetText(	IN int32 tid,
			IN int32 wid,
			IN int32 cid,
			OUT ASCTEXT text,
			IN uint32 bufsz);

extern
BOOL
GuiUpdate(	IN int32 tid,
			IN int32 wid);

extern
BOOL
GuiRenderTextBuffer(IN int32 tid,
					IN int32 wid,
					IN uint8 * txtbuf,
					IN uint32 row,
					IN uint32 column,
					IN uint32 curx,
					IN uint32 cury);

extern
void
GuiFreeMessageData(	IN int32 tid,
					IN void * data);

extern
BOOL
GuiEnableControl(	IN int32 tid,
					IN int32 wid,
					IN int32 cid);

extern
BOOL
GuiDisableControl(	IN int32 tid,
					IN int32 wid,
					IN int32 cid);

extern
BOOL
GuiGetWidth(IN int32 tid,
			IN int32 wid,
			IN int32 cid,
			OUT uint32 * width);

extern
BOOL
GuiGetHeight(	IN int32 tid,
				IN int32 wid,
				IN int32 cid,
				OUT uint32 * height);

extern
BOOL
GuiNewButton(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN CASCTEXT text,
				OUT int32 * cid);

extern
BOOL
GuiNewLabel(IN int32 tid,
			IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN CASCTEXT text,
			OUT int32 * cid);

extern
BOOL
GuiNewList(	IN int32 tid,
			IN int32 wid,
			IN uint32 count,
			IN int32 x,
			IN int32 y,
			IN CASCTEXT text,
			OUT int32 * cid);

extern
BOOL
GuiSetListText(	IN int32 tid,
				IN int32 wid,
				IN int32 cid,
				IN uint32 index,
				IN CASCTEXT text);

extern
BOOL
GuiEnableListItem(	IN int32 tid,
					IN int32 wid,
					IN int32 cid,
					IN uint32 index);

extern
BOOL
GuiDisableListItem(	IN int32 tid,
					IN int32 wid,
					IN int32 cid,
					IN uint32 index);

extern
BOOL
GuiNewEdit(	IN int32 tid,
			IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN uint32 row,
			IN uint32 column,
			IN CASCTEXT text,
			IN uint32 style,
			OUT int32 * cid);

extern
BOOL
GuiNewFileList(	IN int32 tid,
				IN int32 wid,
				IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 vscroll_width,
				IN uint32 color,
				IN uint32 bgcolor,
				IN uint32 colorh,
				IN uint32 bgcolorh,
				OUT int32 * cid);

DEFINE_LOCK_EXTERN(gui)

#endif
