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

typedef struct
{
	int32		wid;
	uint32		type;
	uint32		mouse_x;
	uint32		mouse_y;
	uint32		mouse_btn;
	uint32		key;
	void *		data;
} WindowMessage, * WindowMessagePtr;


extern
BOOL
gui_init(void);

extern
BOOL
gui_is_valid(void);

extern
int32
gui_create_window(	IN int32		tid,
					IN uint32		width,
					IN uint32		height,
					IN uint32		bgcolor,
					IN uint32		style,
					IN CASCTEXT		title);

extern
BOOL
gui_close_window(	IN int32 tid,
					IN int32 wid);

extern
void
gui_close_windows(IN int32 tid);

extern
BOOL
gui_set_window_state(	IN int32 tid,
						IN int32 wid,
						IN uint32 state);

extern
BOOL
gui_get_window_state(	IN int32 tid,
						IN int32 wid,
						IN uint32 * state);

extern
BOOL
gui_set_window_position(IN int32 tid,
						IN int32 wid,
						IN int32 x,
						IN int32 y);

extern
BOOL
gui_get_window_position(IN int32 tid,
						IN int32 wid,
						IN int32 * x,
						IN int32 * y);

extern
BOOL
gui_set_window_size(IN int32 tid,
					IN int32 wid,
					IN uint32 width,
					IN uint32 height);

extern
BOOL
gui_get_window_size(IN int32 tid,
					IN int32 wid,
					IN uint32 * width,
					IN uint32 * height);

extern
BOOL
gui_set_window_title(	IN int32 tid,
						IN int32 wid,
						IN CASCTEXT title);

extern
BOOL
gui_get_window_title(	IN int32 tid,
						IN int32 wid,
						OUT ASCTEXT title);

DEFINE_LOCK_EXTERN(gui)

#endif
