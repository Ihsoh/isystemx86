/**
	@File:			power.c
	@Author:		Ihsoh
	@Date:			2015-09-20
	@Description:
		电源管理窗体。
*/

#include "start.h"
#include "../types.h"
#include "../memory.h"
#include "../vesa.h"
#include "../kernel.h"

#include <ilib/string.h>

#include "../window/control.h"
#include "../window/list.h"
#include "../window/button.h"

#define	_ITEM_COUNT		3
#define	_ITEM_MAX_LEN	20

#define	_WIDTH		(BUTTON_LPADDING + _ITEM_MAX_LEN * ENFONT_WIDTH + BUTTON_RPADDING)
#define	_HEIGHT		(24 * _ITEM_COUNT)

#define	_ITEM_ID_SHUTDOWN		0
#define	_ITEM_ID_REBOOT			1
#define	_ITEM_ID_CANCEL			2

static WindowPtr _window	= NULL;

static ListPtr _lst_power	= NULL;

/**
	@Function:		_control_event
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
_control_event(	IN uint32 id,
				IN uint32 type,
				IN void * param)
{
	if(id == _lst_power->id)
	{
		if(type == BUTTON_LBUP)
			switch(*(uint32 *)param)
			{
				case _ITEM_ID_SHUTDOWN:
					shutdown_system();
					break;
				case _ITEM_ID_REBOOT:
					reboot_system();
					break;
				case _ITEM_ID_CANCEL:
					power_window_hide();
					break;
			}
	}
}

/**
	@Function:		_window_event
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
_window_event(	IN struct Window * window,
				IN struct WindowEventParams * params)
{
	BOOL top = get_top_window() == window;
	if(params->event_type == WINDOW_EVENT_PAINT)
	{
		LIST(_lst_power, &window->workspace, params, TRUE);
	}
}

/**
	@Function:		power_window_init
	@Access:		Public
	@Description:
		初始化电源管理窗体。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
power_window_init(void)
{
	_window = create_window(_WIDTH, _HEIGHT,
							0xff222222,
							WINDOW_STYLE_NO_TITLE | WINDOW_STYLE_NO_WMGR,
							"Power",
							_window_event);
	rect_common_image(&_window->workspace, 0, 0, _WIDTH, _HEIGHT, 0xff222222);
	_lst_power = NEW(List);
	list_init(	_lst_power, 0,
				_ITEM_COUNT,
				0, 0,
				"",
				0xffffffff, 0xff222222, 0xffffffff, 0xff444444,
				_control_event);
	SET_LIST_TEXT(_lst_power, _ITEM_ID_SHUTDOWN,	"      Shutdown      ");
	SET_LIST_TEXT(_lst_power, _ITEM_ID_REBOOT,		"      Reboot        ");
	SET_LIST_TEXT(_lst_power, _ITEM_ID_CANCEL,		"      Cancel        ");
	uint32 ui;
	for(ui = 0; ui < _lst_power->count; ui++)
		_lst_power->buttons[ui].style = BUTTON_STYLE_REFRESH;
	return TRUE;
}

/**
	@Function:		power_window_show
	@Access:		Public
	@Description:
		显示电源管理窗体。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
power_window_show(void)
{
	uint32 w = vesa_get_width();
	uint32 h = vesa_get_height();
	_window->x = w / 2 - _WIDTH / 2;
	_window->y = h / 2 - _HEIGHT / 2;
	_window->state = WINDOW_STATE_SHOW;
	set_top_window(_window);
	return TRUE;
}

/**
	@Function:		power_window_hide
	@Access:		Public
	@Description:
		隐藏电源管理窗体。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
power_window_hide(void)
{
	_window->state = WINDOW_STATE_HIDDEN;
	return TRUE;
}
