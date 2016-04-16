/**
	@File:			start.c
	@Author:		Ihsoh
	@Date:			2015-09-20
	@Description:
		开始菜单。
*/

#include "start.h"
#include "../types.h"
#include "../memory.h"
#include "../config.h"

#include <ilib/string.h>

#include "message.h"
#include "power.h"
#include "taskmgr.h"
#include "run.h"

#include "../window/control.h"
#include "../window/button.h"
#include "../window/list.h"

#define	_ITEM_COUNT		7
#define	_ITEM_MAX_LEN	20

#define	_ITEM_ID_SETTING		0
#define	_ITEM_ID_EXPLORER		1
#define	_ITEM_ID_ABOUT			2
#define	_ITEM_ID_TASKMGR		3
#define	_ITEM_ID_RUN			4
#define	_ITEM_ID_POWER			5
#define	_ITEM_ID_CLOSE			6

#define	_WIDTH		(BUTTON_LPADDING + _ITEM_MAX_LEN * ENFONT_WIDTH + BUTTON_RPADDING)
#define	_HEIGHT		(24 * _ITEM_COUNT)

static WindowPtr _window	= NULL;

static ListPtr _lst_start	= NULL;

/**
	@Function:		_pad
	@Access:		Private
	@Description:
		如果字符串的长度小于MESSAGE_BOX_MAX_CHAR，
		则用空格填充字符串使其长度等于MESSAGE_BOX_MAX_CHAR。
	@Parameters:
		text, ASCTEXT, IN OUT
			指向需要填充空格的字符串的缓冲区的指针。
	@Return:
*/
static
void
_pad(IN OUT ASCTEXT text)
{
	int32 len = (int32)MESSAGE_BOX_MAX_CHAR - (int32)strlen(text);
	int32 i;
	for(i = 0; i < len; i++)
		UtlConcatString(text, MESSAGE_BOX_MAX_CHAR + 1, " ");
}

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
	if(id == _lst_start->id)
	{
		if(type == BUTTON_LBUP)
			switch(*(uint32 *)param)
			{
				case _ITEM_ID_ABOUT:
				{
					ASCCHAR name[KB(1)]		= "ISystem";
					ASCCHAR platform[KB(1)]	= "Unknow platform";
					ASCCHAR version[KB(1)]	= "Unknow version";
					ASCCHAR group[KB(1)]	= "Unknow group";
					config_system_get_string("Name", name, sizeof(name));
					config_system_get_string("Platform", platform, sizeof(platform));
					config_system_get_string("Version", version, sizeof(version));
					config_system_get_string("Group", group, sizeof(group));
					ASCCHAR buffer[KB(1)];
					_pad(name);
					_pad(platform);
					_pad(version);
					UtlCopyString(buffer, sizeof(buffer), name);
					UtlConcatString(buffer, sizeof(buffer), platform);
					UtlConcatString(buffer, sizeof(buffer), version);
					UtlConcatString(buffer, sizeof(buffer), group);
					message_window_show("About System",
										buffer,
										MESSAGE_WINDOW_STYLE_CENTER | MESSAGE_WINDOW_STYLE_TOP,
										0, 0,
										0xff000000, 0xffffffff);
					break;
				}
				case _ITEM_ID_TASKMGR:
				{
					taskmgr_window_show();
					break;
				}
				case _ITEM_ID_RUN:
				{
					run_window_show();
					break;
				}
				case _ITEM_ID_POWER:
					power_window_show();
					break;
				case _ITEM_ID_CLOSE:
					start_window_hide();
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
		LIST(_lst_start, &window->workspace, params, TRUE);
	}
	else if(params->event_type == WINDOW_EVENT_UNFOCUS)
		start_window_hide();
}

/**
	@Function:		start_window_init
	@Access:		Public
	@Description:
		初始化开始菜单。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
start_window_init(void)
{
	_window = create_window(_WIDTH, _HEIGHT,
							0xff222222,
							WINDOW_STYLE_NO_TITLE
								| WINDOW_STYLE_NO_WMGR
								| WINDOW_STYLE_NO_BORDER,
							"Start",
							_window_event);
	rect_common_image(&_window->workspace, 0, 0, _WIDTH, _HEIGHT, 0xff222222);
	_lst_start = NEW(List);
	list_init(	_lst_start, 0,
				_ITEM_COUNT,
				0, 0,
				"",
				0xffffffff, 0xff222222, 0xffffffff, 0xff444444,
				_control_event);
	SET_LIST_TEXT(_lst_start, _ITEM_ID_SETTING,		"Setting           > ");
	SET_LIST_TEXT(_lst_start, _ITEM_ID_EXPLORER,	"Explorer          > ");
	SET_LIST_TEXT(_lst_start, _ITEM_ID_POWER,		"Power             > ");
	SET_LIST_TEXT(_lst_start, _ITEM_ID_TASKMGR,		"Task Manager      > ");
	SET_LIST_TEXT(_lst_start, _ITEM_ID_RUN,			"Run               > ");
	SET_LIST_TEXT(_lst_start, _ITEM_ID_ABOUT,		"About             > ");
	SET_LIST_TEXT(_lst_start, _ITEM_ID_CLOSE,		"Close               ");
	uint32 ui;
	for(ui = 0; ui < _lst_start->count; ui++)
		_lst_start->buttons[ui].style = BUTTON_STYLE_REFRESH;
}

/**
	@Function:		start_window_show
	@Access:		Public
	@Description:
		显示开始菜单。
	@Parameters:
		x, int32, IN
			开始菜单显示的X坐标。
		taskbar_y, int32, IN
			任务栏的Y坐标。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
start_window_show(	IN int32 x,
					IN int32 taskbar_y)
{
	_window->x = x;
	_window->y = taskbar_y - _HEIGHT;
	_window->state = WINDOW_STATE_SHOW;
	set_top_window(_window);
	return TRUE;
}

/**
	@Function:		start_window_hide
	@Access:		Public
	@Description:
		隐藏开始菜单。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
start_window_hide(void)
{
	_window->state = WINDOW_STATE_HIDDEN;
	return TRUE;
}
