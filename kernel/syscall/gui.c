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
	}
}
