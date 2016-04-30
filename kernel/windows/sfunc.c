/**
	@File:			sfunc.c
	@Author:		Ihsoh
	@Date:			2014-12-12
	@Description:
		系统功能的窗体程序。
*/

#include "sfunc.h"
#include "clock.h"
#include "console.h"
#include "../types.h"
#include "../screen.h"
#include "../image.h"
#include "../enfont.h"
#include "../klib.h"
#include "../kernel.h"
#include "../window.h"

static struct Window * sfunc_window;

/**
	@Function:		_WinSfuncEvent
	@Access:		Private
	@Description:
		系统功能窗体的事件处理函数。
	@Parameters:
		window, struct Window *, IN
			事件发生的窗体。
		params, struct WindowEventParams *, IN
			事件的参数。
	@Return:		
*/
static
void
_WinSfuncEvent(struct Window * window, struct WindowEventParams * params)
{
	struct CommonImage * screen = params->screen;
	if(params->event_type == WINDOW_EVENT_PAINT)
	{
		rect_common_image(screen, 0, 0, 320, 200, 0xffffffff);
		if(point_in_rect(	params->mouse_x, 
							params->mouse_y, 
							20, 
							20, 
							strlen("Clock") * ENFONT_WIDTH, 
							ENFONT_HEIGHT))
		{
			rect_common_image(	screen, 
								20, 
								20, 
								strlen("Clock") * ENFONT_WIDTH, 
								ENFONT_HEIGHT, 
								0xffbbbbbb);
			if(KnlIsMouseLeftButtonDown())
				WinClockShow();
		}		
		else
			rect_common_image(	screen, 
								20, 
								20, 
								strlen("Clock") * ENFONT_WIDTH, 
								ENFONT_HEIGHT, 
								0xff999999);
		text_common_image(	screen, 
							20, 
							20, 
							EnfntGetFontDataPtr(), 
							"Clock", 
							strlen("Clock"), 
							0xff000000);
		
		if(point_in_rect(	params->mouse_x, 
							params->mouse_y, 
							20, 
							40, 
							strlen("Console") * ENFONT_WIDTH, 
							ENFONT_HEIGHT))
		{
			rect_common_image(	screen, 
								20, 
								40, 
								strlen("Console") * ENFONT_WIDTH, 
								ENFONT_HEIGHT, 
								0xffbbbbbb);
			if(KnlIsMouseLeftButtonDown())
				WinConsoleShow();
		}		
		else
			rect_common_image(	screen, 
								20, 
								40, 
								strlen("Console") * ENFONT_WIDTH, 
								ENFONT_HEIGHT, 
								0xff999999);
		text_common_image(	screen, 
							20, 
							40, 
							EnfntGetFontDataPtr(), 
							"Console", 
							strlen("Console"), 
							0xff000000);
	}
}

/**
	@Function:		WinSfuncInit
	@Access:		Public
	@Description:
		初始化系统功能窗体程序。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
WinSfuncInit(void)
{
	sfunc_window = ScrCreateWindow(	320, 
									200, 
									0xffffffff, 
									WINDOW_STYLE_MINIMIZE, 
									"System Function", 
									_WinSfuncEvent);
	if(sfunc_window == NULL)
		return FALSE;
	return TRUE;
}

/**
	@Function:		WinSfuncShow
	@Access:		Public
	@Description:
		显示系统功能窗体。
	@Parameters:
	@Return:
*/
void
WinSfuncShow(void)
{
	sfunc_window->state = WINDOW_STATE_SHOW;
}
