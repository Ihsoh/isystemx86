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
gui_clear_messages(	IN int32 tid,
					IN int32 wid);

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
						OUT uint32 * state);

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
						OUT int32 * x,
						OUT int32 * y);

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
					OUT uint32 * width,
					OUT uint32 * height);

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

extern
BOOL
gui_has_key(IN int32 tid,
			IN int32 wid,
			OUT uint32 * key);

extern
BOOL
gui_get_key(IN int32 tid,
			IN int32 wid,
			OUT uint32 * key);

extern
BOOL
gui_get_mouse(	IN int32 tid,
				IN int32 wid,
				OUT int32 * x,
				OUT int32 * y,
				OUT uint32 * button);

extern
BOOL
gui_focused(IN int32 tid,
			IN int32 wid);

extern
BOOL
gui_focus(	IN int32 tid,
			IN int32 wid);

extern
BOOL
gui_get_pixel(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				OUT uint32 * pixel);

extern
BOOL
gui_draw_rect(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN int32 width,
				IN int32 height,
				IN uint32 pixel);

extern
BOOL
gui_draw_image(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN int32 width,
				IN int32 height,
				IN ImagePtr image);

extern
BOOL
gui_draw_text(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN CASCTEXT text,
				IN uint32 color);

extern
BOOL
gui_draw_line(	IN int32 tid,
				IN int32 wid,
				IN int32 startx,
				IN int32 starty,
				IN int32 endx,
				IN int32 endy,
				IN uint32 color);

extern
BOOL
gui_push_message(	IN int32 tid,
					IN int32 wid,
					IN int32 cid,
					IN uint32 type,
					IN void * data,
					IN void * phyaddr_data);

extern
BOOL
gui_pop_message(IN int32 tid,
				IN int32 wid,
				OUT int32 * cid,
				OUT uint32 * type,
				OUT void ** data,
				OUT void ** phyaddr_data);

extern
BOOL
gui_set_text(	IN int32 tid,
				IN int32 wid,
				IN int32 id,
				IN CASCTEXT text);

extern
BOOL
gui_get_text(	IN int32 tid,
				IN int32 wid,
				IN int32 cid,
				OUT ASCTEXT text,
				IN uint32 bufsz);

extern
BOOL
gui_update(	IN int32 tid,
			IN int32 wid);

extern
BOOL
gui_render_text_buffer(	IN int32 tid,
						IN int32 wid,
						IN uint8 * txtbuf,
						IN uint32 row,
						IN uint32 column,
						IN uint32 curx,
						IN uint32 cury);

extern
void
gui_free_msgdata(	IN int32 tid,
					IN void * data);

extern
BOOL
gui_enable_control(	IN int32 tid,
					IN int32 wid,
					IN int32 cid);

extern
BOOL
gui_disable_control(IN int32 tid,
					IN int32 wid,
					IN int32 cid);

extern
BOOL
gui_get_width(	IN int32 tid,
				IN int32 wid,
				IN int32 cid,
				OUT uint32 * width);

extern
BOOL
gui_get_height(	IN int32 tid,
				IN int32 wid,
				IN int32 cid,
				OUT uint32 * height);

extern
BOOL
gui_new_button(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN CASCTEXT text,
				OUT int32 * cid);

extern
BOOL
gui_new_label(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN CASCTEXT text,
				OUT int32 * cid);

extern
BOOL
gui_new_list(	IN int32 tid,
				IN int32 wid,
				IN uint32 count,
				IN int32 x,
				IN int32 y,
				IN CASCTEXT text,
				OUT int32 * cid);

extern
BOOL
gui_set_list_text(	IN int32 tid,
					IN int32 wid,
					IN int32 cid,
					IN uint32 index,
					IN CASCTEXT text);

extern
BOOL
gui_enable_list_item(	IN int32 tid,
						IN int32 wid,
						IN int32 cid,
						IN uint32 index);

extern
BOOL
gui_disable_list_item(	IN int32 tid,
						IN int32 wid,
						IN int32 cid,
						IN uint32 index);

extern
BOOL
gui_new_edit(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN uint32 row,
				IN uint32 column,
				IN CASCTEXT text,
				IN uint32 style,
				OUT int32 * cid);

DEFINE_LOCK_EXTERN(gui)

#endif
