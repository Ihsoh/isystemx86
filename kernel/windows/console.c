/**
	@File:			console.c
	@Author:		Ihsoh
	@Date:			2014-12-11
	@Description:
		控制台窗体程序。
*/

#include "console.h"
#include "../types.h"
#include "../screen.h"
#include "../window.h"

#define	WINDOW_TITLE	"Console"
#define	WINDOW_WIDTH	console_screen_width
#define	WINDOW_HEIGHT	console_screen_height

struct Window * console_window;

/**
	@Function:		_WinConsoleEvent
	@Access:		Private
	@Description:
		控制台窗体程序的事件处理函数。
	@Parameters:
		window, struct Window *, IN
			事件发生的窗体。
		params, struct WindowEventParams *, IN
			事件的参数。
	@Return:	
*/
static
void
_WinConsoleEvent(	IN struct Window * window, 
					IN struct WindowEventParams * params)
{
	struct CommonImage * screen = params->screen;
	if(params->event_type == WINDOW_EVENT_PAINT)
		draw_common_image(	screen, 
							&console_screen_buffer, 
							0, 
							0, 
							console_screen_buffer.width, 
							console_screen_buffer.height);
	else if(params->event_type == WINDOW_EVENT_HIDDEN)
		no_flush_char_buffer();
	else if(params->event_type == WINDOW_EVENT_SHOW)
		flush_char_buffer();
}

/**
	@Function:		WinConsoleInit
	@Access:		Public
	@Description:
		初始化控制台窗体程序。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
WinConsoleInit(void)
{
	console_window = create_window(	WINDOW_WIDTH, 
									WINDOW_HEIGHT, 
									0xffbbbbbb, 
									WINDOW_STYLE_MINIMIZE, 
									WINDOW_TITLE, 
									_WinConsoleEvent);
	if(console_window == NULL)
		return FALSE;
	
	//使窗体初始化在屏幕中央
	uint32 screen_width = vesa_get_width();
	uint32 screen_height = vesa_get_height();
	console_window->x = screen_width / 2 - WINDOW_WIDTH / 2;
	console_window->y = screen_height / 2 - WINDOW_HEIGHT / 2;

	console_window->state = WINDOW_STATE_SHOW;
	return TRUE;
}

/**
	@Function:		WinConsoleShow
	@Access:		Public
	@Description:
		显示控制台窗体。
	@Parameters:
	@Return:	
*/
void
WinConsoleShow(void)
{
	console_window->state = WINDOW_STATE_SHOW;
}
