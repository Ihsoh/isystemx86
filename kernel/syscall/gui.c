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

/**
	@Function:		_ScGuiProcess
	@Access:		Public
	@Description:
		GUI的系统调用处理程序。
	@Parameters:
		func, uint32, IN
			功能号。
		base, uint32, IN
			任务的基地址。该地址为物理地址。
		sparams, struct SParams *, IN OUT
			系统调用参数。
	@Return:	
*/
void
_ScGuiProcess(	IN uint32 func,
				IN uint32 base,
				IN OUT struct SParams * sparams)
{
	int32 tid = sparams->tid;
	switch(func)
	{
		case SCALL_GUI_IS_VALID:
		{
			BOOL r = GuiIsEnabled();
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
			CASCTEXT title = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			int32 wid = GuiCreateWindow(tid, width, height, bgcolor, style, title);
			sparams->param0 = SPARAM(wid);
			break;
		}
		case SCALL_GUI_CLOSE_WINDOW:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			BOOL r = GuiCloseWindow(tid, wid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_WINDOW_STATE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			uint32 state = UINT32_SPARAM(sparams->param1);
			BOOL r = GuiSetWindowStatus(tid, wid, state);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_WINDOW_STATE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			uint32 * state = (uint32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiGetWindowStatus(tid, wid, state);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_WINDOW_POSITION:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			BOOL r = GuiSetWindowPosition(tid, wid, x, y);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_WINDOW_POSITION:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			int32 * x = (int32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param2);
			int32 * y = (int32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiGetWindowPosition(tid, wid, x, y);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_WINDOW_SIZE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			uint32 width = UINT32_SPARAM(sparams->param1);
			uint32 height = UINT32_SPARAM(sparams->param2);
			BOOL r = GuiSetWindowSize(tid, wid, width, height);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_WINDOW_SIZE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			uint32 * width = (uint32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param2);
			uint32 * height = (uint32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiGetWindowSize(tid, wid, width, height);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_WINDOW_TITLE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			CASCTEXT title = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiSetWindowTitle(tid, wid, title);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_WINDOW_TITLE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			ASCTEXT title = (ASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiGetWindowTitle(tid, wid, title);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_HAS_KEY:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			uint32 * key = (uint32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiHasKey(tid, wid, key);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_KEY:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			uint32 * key = (uint32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiGetKey(tid, wid, key);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_MOUSE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param1);
			int32 * x = (int32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param2);
			int32 * y = (int32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param3);
			uint32 * button = (uint32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiGetMouse(tid, wid, x, y, button);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_FOCUSED:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			BOOL r = GuiIsFocused(tid, wid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_FOCUS:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			BOOL r = GuiFocus(tid, wid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_PIXEL:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			uint32 pixel = UINT32_SPARAM(sparams->param3);
			BOOL r = GuiSetPixel(tid, wid, x, y, pixel);
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
			uint32 * pixel = (uint32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiGetPixel(tid, wid, x, y, pixel);
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
			BOOL r = GuiDrawRect(tid, wid, x, y, width, height, pixel);
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
			vptr = TaskmgrConvertLAddrToPAddr(tid, vptr);
			Image img;
			memcpy(&img, vptr, sizeof(Image));
			img.data = (uint8 *)TaskmgrConvertLAddrToPAddr(tid, img.data);
			BOOL r = GuiDrawImage(tid, wid, x, y, width, height, &img);
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
			CASCTEXT text = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			uint32 color = UINT32_SPARAM(sparams->param4);
			BOOL r = GuiDrawText(tid, wid, x, y, text, color);
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
			BOOL r = GuiDrawLine(tid, wid, startx, starty, endx, endy, color);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_MESSAGE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param1);
			int32 * cid = (int32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param2);
			uint32 * type = (uint32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param3);
			void ** data = (void **)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiPopMessage(tid, wid, cid, type, data, NULL);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_TEXT:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param2);
			CASCTEXT text = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiSetText(tid, wid, cid, text);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_TEXT:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param2);
			ASCTEXT text = (ASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			uint32 bufsz = UINT32_SPARAM(sparams->param3);
			BOOL r = GuiGetText(tid, wid, cid, text, bufsz);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_UPDATE:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			BOOL r = GuiUpdate(tid, wid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_RENDER_TEXT_BUFFER:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param1);
			uint8 * txtbuf = (uint8 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			uint32 row = UINT32_SPARAM(sparams->param2);
			uint32 column = UINT32_SPARAM(sparams->param3);
			uint32 curx = UINT32_SPARAM(sparams->param4);
			uint32 cury = UINT32_SPARAM(sparams->param5);
			BOOL r = GuiRenderTextBuffer(tid, wid, txtbuf, row, column, curx, cury);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_FREE_MSGDATA:
		{
			void * data = VOID_PTR_SPARAM(sparams->param0);
			GuiFreeMessageData(tid, data);
			break;
		}
		case SCALL_GUI_ENABLE_CONTROL:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			BOOL r = GuiEnableControl(tid, wid, cid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_DISABLE_CONTROL:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			BOOL r = GuiDisableControl(tid, wid, cid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_WIDTH:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param2);
			uint32 * width = (uint32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiGetWidth(tid, wid, cid, width);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_GET_HEIGHT:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param2);
			uint32 * height = (uint32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiGetHeight(tid, wid, cid, height);
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
			CASCTEXT text = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param4);
			int32 * cid = (int32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiNewButton(tid, wid, x, y, text, cid);
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
			CASCTEXT text = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param4);
			int32 * cid = (int32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiNewLabel(tid, wid, x, y, text, cid);
			sparams->param0 = SPARAM(r);
			break;
		}

		// ============================== List ==============================
		case SCALL_GUI_NEW_LIST:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			uint32 count = UINT32_SPARAM(sparams->param1);
			int32 x = INT32_SPARAM(sparams->param2);
			int32 y = INT32_SPARAM(sparams->param3);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param4);
			CASCTEXT text = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			vptr = VOID_PTR_SPARAM(sparams->param5);
			int32 * cid = (int32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiNewList(tid, wid, count, x, y, text, cid);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_SET_LIST_TEXT:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			uint32 index = UINT32_SPARAM(sparams->param2);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param3);
			CASCTEXT text = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiSetListText(tid, wid, cid, index, text);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_ENABLE_LIST_ITEM:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			uint32 index = UINT32_SPARAM(sparams->param2);
			BOOL r = GuiEnableListItem(tid, wid, cid, index);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_GUI_DISABLE_LIST_ITEM:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 cid = INT32_SPARAM(sparams->param1);
			uint32 index = UINT32_SPARAM(sparams->param2);
			BOOL r = GuiDisableListItem(tid, wid, cid, index);
			sparams->param0 = SPARAM(r);
			break;
		}

		// ============================== Edit ==============================
		case SCALL_GUI_NEW_EDIT:
		{
			int32 wid = INT32_SPARAM(sparams->param0);
			int32 x = INT32_SPARAM(sparams->param1);
			int32 y = INT32_SPARAM(sparams->param2);
			uint32 row = UINT32_SPARAM(sparams->param3);
			uint32 column = UINT32_SPARAM(sparams->param4);
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param5);
			CASCTEXT text = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			uint32 style = UINT32_SPARAM(sparams->param6);
			vptr = VOID_PTR_SPARAM(sparams->param7);
			int32 * cid = (int32 *)TaskmgrConvertLAddrToPAddr(tid, vptr);
			BOOL r = GuiNewEdit(tid, wid, x, y, row, column, text, style, cid);
			sparams->param0 = SPARAM(r);
			break;
		}
	}
}
