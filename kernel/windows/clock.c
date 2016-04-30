/**
	@File:			clock.c
	@Author:		Ihsoh
	@Date:			2014-12-11
	@Description:
		系统内置的时钟窗体程序。
*/

#include "clock.h"
#include "../types.h"
#include "../screen.h"
#include "../cmos.h"
#include "../image.h"
#include "../enfont.h"
#include "../window.h"

#include <ilib/string.h>

#define	WINDOW_TITLE	"Clock"
#define	WINDOW_WIDTH	250
#define	WINDOW_HEIGHT	20

static struct Window * clock_window;
static int8 weeks[][10] = {	"Sunday", 
							"Monday", 
							"Tuesday",
							"Wednesday",
							"Thursday",
							"Friday",
							"Saturday"};

/**
	@Function:		_WinClockEvent
	@Access:		Private
	@Description:
		时钟窗体的事件处理函数。
	@Parameters:
		window, struct Window *, IN
			事件发生的窗体。
		params, struct WindowEventParams *, IN
			事件的参数。
	@Return:		
*/
static
void
_WinClockEvent(	IN struct Window * window, 
				IN struct WindowEventParams * params)
{
	/*
	if(params->event_type == WINDOW_EVENT_PAINT)
	{
		
	}
	struct CommonImage * screen = params->screen;	
	if(params->event_type == WINDOW_EVENT_PAINT)
	{
		rect_common_image(	screen, 
							0, 
							0, 
							WINDOW_WIDTH, 
							WINDOW_HEIGHT, 
							0xffffffff);
		struct CMOSDateTime dt;
		CmosGetDateTime(&dt);
		char buffer[1024] = "";
		char temp[10];
		
		UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.year));
		UtlConcatString(buffer, sizeof(buffer), "-");
		UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.month));
		UtlConcatString(buffer, sizeof(buffer), "-");
		UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.day));
		UtlConcatString(buffer, sizeof(buffer), " ");
		UtlConcatString(buffer, sizeof(buffer), weeks[dt.day_of_week]);
		UtlConcatString(buffer, sizeof(buffer), " ");
		UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.hour));
		UtlConcatString(buffer, sizeof(buffer), ":");
		UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.minute));
		UtlConcatString(buffer, sizeof(buffer), ":");
		UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.second));
		text_common_image(	screen,
							10,
							0,
							EnfntGetFontDataPtr(),
							buffer,
							strlen(buffer),
							0xff000000);
	}
	*/
}

/**
	@Function:		WinClockInit
	@Access:		Public
	@Description:
		初始化时钟窗体程序。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
WinClockInit(void)
{
	clock_window = ScrCreateWindow(	WINDOW_WIDTH, 
									WINDOW_HEIGHT, 
									0xffffffff,
									WINDOW_STYLE_MINIMIZE, 
									WINDOW_TITLE, 
									_WinClockEvent);
	if(clock_window == NULL)
		return FALSE;

	//使窗体初始化在屏幕右上角
	uint32 screen_width = VesaGetWidth();
	clock_window->x = screen_width - WINDOW_WIDTH;
	clock_window->y = 0;

	//clock_window->state = WINDOW_STATE_SHOW;
	return TRUE;
}

/**
	@Function:		WinClockShow
	@Access:		Public
	@Description:
		显示时钟窗体。
	@Parameters:
	@Return:
*/
void
WinClockShow(void)
{
	clock_window->state = WINDOW_STATE_SHOW;
}
