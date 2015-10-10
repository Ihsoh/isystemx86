/**
	@File:			gui.c
	@Author:		Ihsoh
	@Date:			2015-10-04
	@Description:
		GUI的系统调用。
*/

#include "gui.h"
#include "../types.h"
#include "../sparams.h"
#include "../window.h"
#include "../memory.h"
#include "../gui.h"
#include "../image.h"

void
system_call_gui(IN uint32 func,
				IN uint32 base,
				IN OUT struct SParams * sparams)
{
	int32 tid = sparams->tid;
	switch(func)
	{
		case SCALL_GUI_IS_VALID:
		{
			BOOL r = gui_is_valid();
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_CREATE_WINDOW:
		{
			uint32 width = UINT32_SPARAM(sparams->param0);
			uint32 height = UINT32_SPARAM(sparams->param1);
			uint32 bgcolor = UINT32_SPARAM(sparams->param2);
			uint32 style = UINT32_SPARAM(sparams->param3);
			void * vptr = VOID_PTR_SPARAM(sparams->param4);
			CASCTEXT title = (CASCTEXT)get_physical_address(tid, vptr);
			int32 wid = gui_create_window(tid, width, height, bgcolor, style, title);
			sparams->param0 = SPARAM(wid);
			break;
		}
		case SCALL_GUI_CLOSE_WINDOW:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			BOOL r = gui_close_window(tid, wid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_WINDOW_STATE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			uint32 state = UINT32_SPARAM(sparams->param1);
			BOOL r = gui_set_window_state(tid, wid, state);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_WINDOW_STATE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			uint32 * state = (uint32 *)get_physical_address(tid, vptr);
			BOOL r = gui_get_window_state(tid, wid, state);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_WINDOW_POSITION:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			BOOL r = gui_set_window_position(tid, wid, x, y);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_WINDOW_POSITION:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			int32 * x = (int32 *)get_physical_address(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param2);
			int32 * y = (int32 *)get_physical_address(tid, vptr);
			BOOL r = gui_get_window_position(tid, wid, x, y);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_WINDOW_SIZE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			uint32 width = UINT32_SPARAM(sparams->param1);
			uint32 height = UINT32_SPARAM(sparams->param2);
			BOOL r = gui_set_window_size(tid, wid, width, height);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_WINDOW_SIZE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			uint32 * width = (uint32 *)get_physical_address(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param2);
			uint32 * height = (uint32 *)get_physical_address(tid, vptr);
			BOOL r = gui_get_window_size(tid, wid, width, height);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_WINDOW_TITLE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			CASCTEXT title = (CASCTEXT)get_physical_address(tid, vptr);
			BOOL r = gui_set_window_title(tid, wid, title);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_WINDOW_TITLE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			ASCTEXT title = (ASCTEXT)get_physical_address(tid, vptr);
			BOOL r = gui_get_window_title(tid, wid, title);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_HAS_KEY:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			uint32 * key = (uint32 *)get_physical_address(tid, vptr);
			BOOL r = gui_has_key(tid, wid, key);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_KEY:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			uint32 * key = (uint32 *)get_physical_address(tid, vptr);
			BOOL r = gui_get_key(tid, wid, key);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_MOUSE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param1);
			int32 * x = (int32 *)get_physical_address(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param2);
			int32 * y = (int32 *)get_physical_address(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param3);
			uint32 * button = (uint32 *)get_physical_address(tid, vptr);
			BOOL r = gui_get_mouse(tid, wid, x, y, button);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_FOCUSED:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			BOOL r = gui_focused(tid, wid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_FOCUS:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			BOOL r = gui_focus(tid, wid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_PIXEL:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			uint32 pixel = UINT32_SPARAM(sparams->param3);
			BOOL r = gui_set_pixel(tid, wid, x, y, pixel);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_PIXEL:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param3);
			uint32 * pixel = (uint32 *)get_physical_address(tid, vptr);
			BOOL r = gui_get_pixel(tid, wid, x, y, pixel);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_DRAW_RECT:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			int32 width = INT32_SPARAM(sparams->param3);
			int32 height = INT32_SPARAM(sparams->param4);
			uint32 pixel = UINT32_SPARAM(sparams->param5);
			BOOL r = gui_draw_rect(tid, wid, x, y, width, height, pixel);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_DRAW_IMAGE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			int32 width = INT32_SPARAM(sparams->param3);
			int32 height = INT32_SPARAM(sparams->param4);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param5);
			vptr = get_physical_address(tid, vptr);
			Image img;
			memcpy(&img, vptr, sizeof(Image));
			img.data = (uint8 *)get_physical_address(tid, img.data);
			BOOL r = gui_draw_image(tid, wid, x, y, width, height, &img);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_DRAW_TEXT:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param3);
			CASCTEXT text = (CASCTEXT)get_physical_address(tid, vptr);
			uint32 color = UINT32_SPARAM(sparams->param4);
			BOOL r = gui_draw_text(tid, wid, x, y, text, color);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_DRAW_LINE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 startx = INT32_SPARAM(sparams->param1);
			int32 starty = INT32_SPARAM(sparams->param2);
			int32 endx = INT32_SPARAM(sparams->param3);
			int32 endy = INT32_SPARAM(sparams->param4);
			uint32 color = UINT32_SPARAM(sparams->param5);
			BOOL r = gui_draw_line(tid, wid, startx, starty, endx, endy, color);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_MESSAGE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param1);
			int32 * cid = (int32 *)get_physical_address(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param2);
			uint32 * type = (uint32 *)get_physical_address(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param3);
			void ** data = (void **)get_physical_address(tid, vptr);
			BOOL r = gui_pop_message(tid, wid, cid, type, data, NULL);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_TEXT:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param2);
			CASCTEXT text = (CASCTEXT)get_physical_address(tid, vptr);
			BOOL r = gui_set_text(tid, wid, cid, text);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_TEXT:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param2);
			ASCTEXT text = (ASCTEXT)get_physical_address(tid, vptr);
			uint32 bufsz = UINT32_SPARAM(sparams->param3);
			BOOL r = gui_get_text(tid, wid, cid, text, bufsz);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_UPDATE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			BOOL r = gui_update(tid, wid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_RENDER_TEXT_BUFFER:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param1);
			uint8 * txtbuf = (uint8 *)get_physical_address(tid, vptr);
			uint32 row = UINT32_SPARAM(sparams->param2);
			uint32 column = UINT32_SPARAM(sparams->param3);
			uint32 curx = UINT32_SPARAM(sparams->param4);
			uint32 cury = UINT32_SPARAM(sparams->param5);
			BOOL r = gui_render_text_buffer(tid, wid, txtbuf, row, column, curx, cury);
			sparams->param0 = SPARAM(r);
			break;
		}

		// ============================== Button ==============================
		case SCALL_GUI_NEW_BUTTON:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param3);
			CASCTEXT text = (CASCTEXT)get_physical_address(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param4);
			int32 * cid = (int32 *)get_physical_address(tid, vptr);
			BOOL r = gui_new_button(tid, wid, x, y, text, cid);
			sparams->param0 = SPARAM(r);
			break;
		}

		// ============================== Label ==============================
		case SCALL_GUI_NEW_LABEL:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param3);
			CASCTEXT text = (CASCTEXT)get_physical_address(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param4);
			int32 * cid = (int32 *)get_physical_address(tid, vptr);
			BOOL r = gui_new_label(tid, wid, x, y, text, cid);
			sparams->param0 = SPARAM(r);
			break;
		}
	}
}
