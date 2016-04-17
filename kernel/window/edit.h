/**
	@File:			edit.h
	@Author:		Ihsoh
	@Date:			2015-12-11
	@Description:
		窗体控件 - Edit。
*/

#ifndef	_WINDOW_EDIT_H_
#define	_WINDOW_EDIT_H_

#include "control.h"

#include "../types.h"
#include "../image.h"
#include "../window.h"

#define	MAX_EDIT_COLUMN				1024			// 列数的最大值。
#define	MAX_EDIT_ROW				2048			// 行数的最大值。

typedef	struct
{
	uint32				id;
	int32				type;
	int32				uwid;							// User Window ID。
	int32				uwcid;							// User Window Control ID。
	uint32				x;
	uint32				y;
	BOOL				enabled;
	uint32				style;
	ControlEvent		event;

	BOOL				_forced_flush;

	uint32 column, row;						// 文本编辑区的列和行。
	uint32 scr_column, scr_row;				// 屏幕的列和行。
	uint16 cursor_x, cursor_y;				// 光标的坐标。
	ASCCHAR * content_buffer;				// 内容缓冲区。
	uint8 * screen_buffer;					// 屏幕缓冲区。
	uint32 top_column, top_row;				// 屏幕缓冲区左上角相对于内容缓冲区的偏移量。
	uint32 total_line;						// 总行数。
} Edit, * EditPtr;

#define	EDIT_STYLE_REFRESH		0x00000001
#define	EDIT_STYLE_SINGLE_LINE	0x00000002
#define	EDIT_STYLE_READONLY		0x00000004

#define	INIT_EDIT(_edit, _x, _y, _row, _column, _style, _event)	\
	(CtrlEditInit((_edit), 0, (_x), (_y), (_row), (_column), (_style), (_event)))

#define	UNINIT_EDIT(_edit)	\
	(CtrlEditUninit((_edit)))

#define	NMLEDIT(_edit, _image, _params, _top)	\
	(CtrlEditUpdate((_edit), (_image), (_params), (_top)))

#define	GET_EDIT_TEXT(_edit, _text, _size)	\
	(CtrlEditGetText((_edit), (_text), (_size)))

#define	SET_EDIT_TEXT(_edit, _text)	\
	(CtrlEditSetText((_edit), (_text)))

#define	APPEND_EDIT_TEXT(_edit, _text)	\
	(CtrlEditAppendText((_edit), (_text)))

#define	GET_EDIT_WIDTH(_edit)	\
	(CtrlEditGetWidth((_edit)))

#define	GET_EDIT_HEIGHT(_edit)	\
	(CtrlEditGetHeight((_edit)))

extern
BOOL
CtrlEditInit(	OUT EditPtr edit,
				IN uint32 id,
				IN uint32 x,
				IN uint32 y,
				IN uint32 row,
				IN uint32 column,
				IN uint32 style,
				IN ControlEvent event);

extern
void
CtrlEditUninit(IN EditPtr edit);

extern
BOOL
CtrlEditUpdate(	IN OUT EditPtr edit,
				OUT ImagePtr image,
				IN WindowEventParamsPtr params,
				BOOL top);

extern
BOOL
CtrlEditGetText(	IN EditPtr edit,
				OUT ASCTEXT text,
				IN uint32 size);

extern
BOOL
CtrlEditSetText(	IN OUT EditPtr edit,
				IN CASCTEXT text);

extern
BOOL
CtrlEditAppendText(	IN OUT EditPtr edit,
					IN CASCTEXT text);

extern
uint32
CtrlEditGetWidth(IN EditPtr edit);

extern
uint32
CtrlEditGetHeight(IN EditPtr edit);

#endif
