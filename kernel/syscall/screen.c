/**
	@File:			screen.c
	@Author:		Ihsoh
	@Date:			2014-7-28
	@Description:
		屏幕的系统调用处理程序。
*/

#include "screen.h"
#include "../types.h"
#include "../sparams.h"
#include "../screen.h"
#include "../tasks.h"
#include "../vesa.h"
#include "../kernel.h"
#include "../image.h"

/**
	@Function:		_ScScrProcess
	@Access:		Public
	@Description:
		屏幕的系统调用处理程序。
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
_ScScrProcess(	IN uint32 func,
				IN uint32 base,
				IN OUT struct SParams * sparams)
{
	switch(func)
	{
		//打印一个字符
		//
		//参数:
		//	Param0=字符
		case SCALL_PRINT_CHAR:
		{
			ScrPrintChar(INT8_SPARAM(sparams->param0));
			break;
		}
		//打印一个字符. 并指定属性
		//
		//参数:
		//	Param0=字符
		//	Param1=属性
		case SCALL_PRINT_CHAR_P:
		{		
			ScrPrintCharWithProperty(INT8_SPARAM(sparams->param0), UINT8_SPARAM(sparams->param1));
			break;
		}		
		//打印字符串
		//
		//参数:
		//	Param0=缓冲区地址(相对于调用程序空间的偏移地址)
		case SCALL_PRINT_STR:
		{
			const int8 * str = (const int8 *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			ScrPrintString(str);
			break;
		}		
		//打印字符串. 并指定属性
		//
		//参数:
		//	Param0=缓冲区地址(相对于调用程序空间的偏移地址)
		//	Param1=属性
		case SCALL_PRINT_STR_P:
		{
			const int8 * str = (const int8 *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			ScrPrintStringWithProperty(str, UINT8_SPARAM(sparams->param1));
			break;
		}
		//清屏
		//
		case SCALL_CLEAR_SCREEN:
		{
			ScrClearScreen();
			break;
		}		
		//设置光标位置
		//
		//参数:
		//	Param0=X
		//	Param1=Y
		case SCALL_SET_CURSOR:
		{		
			ScrSetConsoleCursor(UINT16_SPARAM(sparams->param0), UINT16_SPARAM(sparams->param1));
			break;
		}
		//读取光标位置
		//
		//返回值:
		//	Param0=X
		//	Param1=Y
		case SCALL_GET_CURSOR:
		{
			uint16 x, y;
			ScrGetConsoleCursor(&x, &y);
			sparams->param0 = SPARAM(x);
			sparams->param1 = SPARAM(y);
			break;
		}
		//确认Vesa是否可用
		//
		//返回值:
		//	Param0=1为可用, 否则不可用
		case SCALL_VESA_IS_VALID:
		{
			BOOL r = VesaIsEnabled();
			sparams->param0 = SPARAM(r);
			break;
		}
		//获取屏幕大小
		//
		//返回值:
		//	Param0=宽度
		//	Param1=高度
		case SCALL_GET_SCREEN_SIZE:
		{
			uint32 width = VesaGetWidth();
			uint32 height = VesaGetHeight();
			sparams->param0 = SPARAM(width);
			sparams->param1 = SPARAM(height);
			break;
		}
		//设置屏幕像素
		//
		//参数:
		//	Param0=X
		//	Param1=Y
		case SCALL_SET_SCREEN_PIXEL:
		{
			uint32 x = UINT32_SPARAM(sparams->param0);
			uint32 y = UINT32_SPARAM(sparams->param1);
			uint32 color = UINT32_SPARAM(sparams->param2);
			VesaSetPixel(x, y, color);
			break;
		}
		//获取屏幕像素
		//
		//参数:
		//	Param0=X
		//	Param1=Y
		//返回值:
		//	Param0=像素
		case SCALL_GET_SCREEN_PIXEL:
		{
			uint32 x = UINT32_SPARAM(sparams->param0);
			uint32 y = UINT32_SPARAM(sparams->param1);
			uint32 pixel = VesaGetPixel(x, y);
			sparams->param0 = SPARAM(pixel);
			break;
		}
		//画位图
		//
		//参数:
		//	Param0=X
		//	Param1=Y
		//	Param2=宽度
		//	Param3=高度
		//	Param4=位图地址(相对于调用程序空间的偏移地址)
		case SCALL_DRAW_SCREEN_IMAGE:
		{
			uint32 x = UINT32_SPARAM(sparams->param0);
			uint32 y = UINT32_SPARAM(sparams->param1);
			uint32 width = UINT32_SPARAM(sparams->param2);
			uint32 height = UINT32_SPARAM(sparams->param3);
			uint32 * image = UINT32_PTR_SPARAM(TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param4)));
			VesaDrawImage(x, y, width, height, image);
			break;
		}
		//获取文本屏幕的列数和行数
		//
		//返回值:
		//	Param0=列数
		//	Param1=行数
		case SCALL_GET_TEXT_SCREEN_SIZE:
		{
			uint32 column = COLUMN;
			uint32 row = ROW;
			sparams->param0 = SPARAM(column);
			sparams->param1 = SPARAM(row);
			break;
		}
		//锁定光标
		//
		case SCALL_LOCK_CURSOR:
		{
			ScrLockConsoleCursor();
			break;
		}
		//解锁光标
		//
		case SCALL_UNLOCK_CURSOR:
		{
			ScrUnlockConsoleCursor();
			break;
		}
		case SCALL_SET_TARGET_SCREEN:
		{
			if(target_screen != NULL)
				MemFree(target_screen);
			struct CommonImage * ts = TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			if(ts == NULL)
			{
				target_screen = NULL;				
				ScrClearScreen();			
			}			
			else
			{				
				struct CommonImage * new = MemAlloc(sizeof(struct CommonImage));
				memcpy(new, ts, sizeof(struct CommonImage));
				new->data = TaskmgrConvertLAddrToPAddr(sparams->tid, new->data);
				target_screen = new;
				ScrClearScreen();
			}	
			break;
		}
		case SCALL_CREATE_WINDOW:
		{
			uint32 width = UINT32_SPARAM(sparams->param0);
			uint32 height = UINT32_SPARAM(sparams->param1);
			uint32 bgcolor = UINT32_SPARAM(sparams->param2);
			uint32 style = UINT32_SPARAM(sparams->param3);
			int8 * title = TaskmgrConvertLAddrToPAddr(sparams->tid, 
												INT8_PTR_SPARAM(sparams->param4));
			WindowEvent * event = NULL;
			struct Window * window;
			window = ScrCreateWindow(width, height, bgcolor, style, title, event);
			if(window == NULL)
			{
				uint32 id = 0;
				sparams->param0 = SPARAM(id);
			}
			else
				sparams->param0 = SPARAM(window->id);
			break;
		}
		case SCALL_DESTROY_WINDOW:
		{
			uint32 id = UINT32_SPARAM(sparams->param0);
			ScrDestroyWindow((struct Window *)id);
			break;
		}
		case SCALL_SHOW_WINDOW:
		{
			uint32 id = UINT32_SPARAM(sparams->param0);
			struct Window * window = (struct Window *)id;
			window->state = WINDOW_STATE_SHOW;
			break;
		}
		case SCALL_HIDE_WINDOW:
		{
			uint32 id = UINT32_SPARAM(sparams->param0);
			struct Window * window = (struct Window *)id;
			window->state = WINDOW_STATE_HIDDEN;
			break;
		}
		case SCALL_WINDOW_GET_KEY:
		{
			uint32 id = UINT32_SPARAM(sparams->param0);
			struct Window * window = (struct Window *)id;
			uint8 key = WinmgrGetKey(window);
			sparams->param0 = SPARAM(key);
			break;
		}
		case SCALL_DRAW_WINDOW:
		{
			uint32 id = UINT32_SPARAM(sparams->param0);
			struct Window * window = (struct Window *)id;
			struct CommonImage * image;
			image = TaskmgrConvertLAddrToPAddr(	sparams->tid, 
											VOID_PTR_SPARAM(sparams->param1));
			struct CommonImage new;
			memcpy(&new, image, sizeof(struct CommonImage));
			new.data = TaskmgrConvertLAddrToPAddr(	sparams->tid, 
												VOID_PTR_SPARAM(new.data));
			BOOL r = draw_common_image(&window->workspace, &new, 0, 0, new.width, new.height);
			sparams->param0 = SPARAM(r);
			break;
		}
		//打印一个字符到标准错误。
		//
		//参数:
		//	Param0=字符。
		case SCALL_PRINT_ERR_CHAR:
		{
			ScrPrintCharToStderr(INT8_SPARAM(sparams->param0));
			break;
		}
		//打印一个字符到标准错误。并指定属性。
		//
		//参数:
		//	Param0=字符。
		//	Param1=属性。
		case SCALL_PRINT_ERR_CHAR_P:
		{		
			ScrPrintCharToStderrWithProperty(INT8_SPARAM(sparams->param0), UINT8_SPARAM(sparams->param1));
			break;
		}		
		//打印字符串到标准错误。
		//
		//参数:
		//	Param0=缓冲区地址(相对于调用程序空间的偏移地址)。
		case SCALL_PRINT_ERR_STR:
		{
			const int8 * str = (const int8 *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			ScrPrintStringToStderr(str);
			break;
		}		
		//打印字符串到标准错误。并指定属性。
		//
		//参数:
		//	Param0=缓冲区地址(相对于调用程序空间的偏移地址)。
		//	Param1=属性。
		case SCALL_PRINT_ERR_STR_P:
		{
			const int8 * str = (const int8 *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			ScrPrintStringToStderrWithProperty(str, UINT8_SPARAM(sparams->param1));
			break;
		}
		//刷新屏幕。
		//
		case SCALL_FLUSH_SCREEN:
		{
			ScrFlushScreen();
			break;
		}
		//写控制台字符缓冲区。
		//
		case SCALL_WRITE_CONSOLE_BUFFER:
		{
			const uint8 * buffer = (const uint8 *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			uint32 size = UINT32_SPARAM(sparams->param1);
			BOOL r = ScrWriteConsoleBuffer(buffer, size);
			sparams->param0 = SPARAM(r);
			break;
		}
		case SCALL_RENDER_TEXT_BUFFER:
		{
			void * vptr = NULL;
			vptr = VOID_PTR_SPARAM(sparams->param0);
			vptr = TaskmgrConvertLAddrToPAddr(sparams->tid, vptr);
			Image image;
			memcpy(&image, vptr, sizeof(Image));
			image.data = (uint8 *)TaskmgrConvertLAddrToPAddr(sparams->tid, image.data);
			vptr = VOID_PTR_SPARAM(sparams->param1);
			uint8 * txtbuf = (uint8 *)TaskmgrConvertLAddrToPAddr(sparams->tid, vptr);
			uint32 row = UINT32_SPARAM(sparams->param2);
			uint32 column = UINT32_SPARAM(sparams->param3);
			uint32 curx = UINT32_SPARAM(sparams->param4);
			uint32 cury = UINT32_SPARAM(sparams->param5);
			BOOL r = ScrRenderTextBuffer(&image, txtbuf, row, column, curx, cury);
			sparams->param0 = SPARAM(r);
			break;
		}
	}
}
