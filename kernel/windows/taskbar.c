/**
	@File:			taskbar.c
	@Author:		Ihsoh
	@Date:			2015-09-19
	@Description:
		任务栏。
*/

#include "taskbar.h"
#include "../types.h"
#include "../screen.h"
#include "../cmos.h"
#include "../image.h"
#include "../enfont.h"
#include "../window.h"
#include "../vesa.h"
#include "../memory.h"
#include "../timer.h"

#include "start.h"

#include <ilib/string.h>

#include "../window/control.h"
#include "../window/button.h"
#include "../window/label.h"

#define	_HEIGHT	TASKBAR_HEIGHT

#define	_BTN_TIME_WIDTH		250

#define	_MAX_TIME_STR_LEN	26

static WindowPtr _window	= NULL;

static ButtonPtr _btn_start	= NULL;
static ButtonPtr _btn_time	= NULL;
static ButtonPtr _btn_wmgr	= NULL;

static TimerPtr _timer 		= NULL;

static int8 _weeks[][10] 	= {	"Sunday", 
								"Monday", 
								"Tuesday",
								"Wednesday",
								"Thursday",
								"Friday",
								"Saturday"};

/**
	@Function:		_WinTskbrControlEvent
	@Access:		Private
	@Description:
		控件的事件函数。
	@Parameters:
		id, uint32, IN
			控件ID。
		type, uint32, IN
			事件类型。
		param, void *, IN
			事件附带的参数。
	@Return:
*/
static
void
_WinTskbrControlEvent(	IN uint32 id,
						IN uint32 type,
						IN void * param)
{
	if(id == _btn_start->id)
	{
		if(type == BUTTON_LBUP)
		{
			WinStartShow(_btn_start->x, _window->y);
		}
	}
	else if(id == _btn_wmgr->id)
	{
		if(type == BUTTON_LBUP)
		{
			WinWmgrShow(_btn_wmgr->x, _window->y);
		}
	}
}

/**
	@Function:		_WinTskbrEvent
	@Access:		Private
	@Description:
		窗体的事件函数。
	@Parameters:
		window, struct Window *, IN
			指向窗体对象的指针。
		params, struct WindowEventParams *, IN
			指向窗体事件参数对象的指针。
	@Return:
*/
static
void
_WinTskbrEvent(	IN struct Window * window,
				IN struct WindowEventParams * params)
{
	BOOL top = get_top_window() == window;
	if(params->event_type == WINDOW_EVENT_PAINT)
	{
		BUTTON(_btn_start, &window->workspace, params, top);
		BUTTON(_btn_wmgr, &window->workspace, params, top);
		BUTTON(_btn_time, &window->workspace, params, top);
	}
}

/**
	@Function:		_WinTskbrTimerEvent
	@Access:		Private
	@Description:
		定时器事件函数。
	@Parameters:
	@Return:
*/
static
void
_WinTskbrTimerEvent(void)
{
	struct CMOSDateTime dt;
	CmosGetDateTime(&dt);
	ASCCHAR buffer[1024] = "";
	ASCCHAR temp[10];
	UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.year));
	UtlConcatString(buffer, sizeof(buffer), "-");
	UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.month));
	UtlConcatString(buffer, sizeof(buffer), "-");
	UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.day));
	UtlConcatString(buffer, sizeof(buffer), " ");
	UtlConcatString(buffer, sizeof(buffer), _weeks[dt.day_of_week]);
	UtlConcatString(buffer, sizeof(buffer), " ");
	UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.hour));
	UtlConcatString(buffer, sizeof(buffer), ":");
	UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.minute));
	UtlConcatString(buffer, sizeof(buffer), ":");
	UtlConcatString(buffer, sizeof(buffer), itos(temp, dt.second));
	uint32 ui;
	uint32 len = strlen(buffer);
	for(ui = 0; ui < _MAX_TIME_STR_LEN - len; ui++)
		UtlConcatString(buffer, sizeof(buffer), " ");
	SET_BUTTON_TEXT(_btn_time, buffer);
}

/**
	@Function:		WinTskbrInit
	@Access:		Public
	@Description:
		初始化任务栏。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
WinTskbrInit(void)
{
	uint32 w = vesa_get_width();
	uint32 h = vesa_get_height();
	_window = create_window(w, _HEIGHT,
							0xff000000,
							WINDOW_STYLE_NO_TITLE 
								| WINDOW_STYLE_NO_WMGR
								| WINDOW_STYLE_NO_BORDER,
							"Taskbar",
							_WinTskbrEvent);
	rect_common_image(&_window->workspace, 0, 0, w, _HEIGHT, 0xff222222);
	_window->y = h - _HEIGHT;

	_btn_start = NEW(Button);
	CtrlBtnInit(_btn_start, 0,
				0, 0,
				"ISystem",
				0xffffffff, 0xff222222, 0xffffffff, 0xff444444,
				_WinTskbrControlEvent,
				0, _HEIGHT);
	_btn_start->style = BUTTON_STYLE_REFRESH;

	_btn_wmgr = NEW(Button);
	CtrlBtnInit(_btn_wmgr, 0,
				_btn_start->width, 0,
				"Window",
				0xffffffff, 0xff222222, 0xffffffff, 0xff444444,
				_WinTskbrControlEvent,
				0, _HEIGHT);
	_btn_wmgr->style = BUTTON_STYLE_REFRESH;

	_btn_time = NEW(Button);
	CtrlBtnInit(_btn_time, 0,
				w - _BTN_TIME_WIDTH, 0,
				"Time",
				0xffffffff, 0xff222222, 0xffffffff, 0xff444444,
				_WinTskbrControlEvent,
				_BTN_TIME_WIDTH, 0);
	_btn_time->style = BUTTON_STYLE_REFRESH;

	_timer = timer_new(800, _WinTskbrTimerEvent);
	timer_start(_timer);

	_window->state = WINDOW_STATE_SHOW;
	return TRUE;
}
