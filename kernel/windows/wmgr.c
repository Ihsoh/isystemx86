/**
	@File:			wmgr.c
	@Author:		Ihsoh
	@Date:			2015-08-15
	@Description:
		窗体管理器。
*/

#include "wmgr.h"
#include "../types.h"
#include "../memory.h"
#include "../config.h"
#include "../screen.h"

#include <ilib/string.h>

#include "../window/control.h"
#include "../window/button.h"
#include "../window/list.h"

#define	_ITEM_COUNT		10
#define	_ITEM_MAX_LEN	30

#define	_ITEM_ID_UP		0
#define	_ITEM_ID_BEGIN	(_ITEM_ID_UP + 1)
#define	_ITEM_ID_END	(_ITEM_COUNT - 3)
#define	_ITEM_ID_DOWN	(_ITEM_COUNT - 2)
#define	_ITEM_ID_CLOSE	(_ITEM_COUNT - 1)

#define	_TITLE_COUNT	(_ITEM_ID_END - _ITEM_ID_BEGIN + 1)

#define	_WIDTH		(BUTTON_LPADDING + _ITEM_MAX_LEN * ENFONT_WIDTH + BUTTON_RPADDING)
#define	_HEIGHT		(24 * _ITEM_COUNT)

static WindowPtr _window	= NULL;

static ListPtr _lst_wmgr	= NULL;

static uint32 _offset		= 0;

static WindowPtr _windows[MAX_WINDOW_COUNT];

/**
	@Function:		_get_windows
	@Access:		Private
	@Description:
		获取指向不包含WINDOW_STYLE_NO_WMGR样式，并且被隐藏的窗体的数组的指针。
	@Parameters:
		count, uint32 *, OUT
			符合要求的窗体的数量。
	@Return:
		WindowPtr *
			指向不包含WINDOW_STYLE_NO_WMGR样式，并且被隐藏的窗体的数组的指针。
*/
static
WindowPtr *
_get_windows(OUT uint32 * count)
{
	uint32 window_count = 0;
	WindowPtr * windows = get_windows(&window_count);
	*count = 0;
	uint32 ui;
	for(ui = 0; ui < window_count; ui++)
		if(	!(windows[ui]->style & WINDOW_STYLE_NO_WMGR)
			&& windows[ui]->state == WINDOW_STATE_HIDDEN)
			_windows[(*count)++] = windows[ui];
	return _windows;
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
	if(id == _lst_wmgr->id)
	{
		if(type == BUTTON_LBUP)
		{
			uint32 id = *(uint32 *)param;
			switch(id)
			{
				case _ITEM_ID_UP:
					if(_offset > 0)
						_offset--;
					wmgr_window_update(_offset);
					break;
				case _ITEM_ID_DOWN:
				{
					uint32 window_count = 0;
					_get_windows(&window_count);
					if(_offset + _TITLE_COUNT < window_count)
						_offset++;
					wmgr_window_update(_offset);
					break;
				}
				case _ITEM_ID_CLOSE:
					wmgr_window_hide();
					break;
				default:
					if(id >= _ITEM_ID_BEGIN && id <= _ITEM_ID_END)
					{
						uint32 windex = _offset + id - _ITEM_ID_BEGIN;
						uint32 window_count = 0;
						WindowPtr * windows = _get_windows(&window_count);
						if(windex < window_count)
						{
							wmgr_window_hide();
							windows[windex]->state = WINDOW_STATE_SHOW;
							set_top_window(windows[windex]);
						}
					}
					break;
			}
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
		LIST(_lst_wmgr, &window->workspace, params, TRUE);
	}
	else if(params->event_type == WINDOW_EVENT_UNFOCUS)
		wmgr_window_hide();
}

/**
	@Function:		wmgr_window_init
	@Access:		Public
	@Description:
		初始化窗体管理器。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
wmgr_window_init(void)
{
	_window = create_window(_WIDTH, _HEIGHT,
							0xff222222,
							WINDOW_STYLE_NO_TITLE
								| WINDOW_STYLE_NO_WMGR
								| WINDOW_STYLE_NO_BORDER,
							"Window Manager",
							_window_event);
	rect_common_image(&_window->workspace, 0, 0, _WIDTH, _HEIGHT, 0xff222222);
	_lst_wmgr = NEW(List);
	list_init(	_lst_wmgr, 0,
				_ITEM_COUNT,
				0, 0,
				"",
				0xffffffff, 0xff222222, 0xffffffff, 0xff444444,
				_control_event);
	SET_LIST_TEXT(_lst_wmgr, _ITEM_ID_UP, 		"Up                            ");
	SET_LIST_TEXT(_lst_wmgr, _ITEM_ID_DOWN, 	"Down                          ");
	SET_LIST_TEXT(_lst_wmgr, _ITEM_ID_CLOSE,	"Close                         ");
	uint32 ui;
	for(ui = 0; ui < _lst_wmgr->count; ui++)
		_lst_wmgr->buttons[ui].style = BUTTON_STYLE_REFRESH;
	for(ui = _ITEM_ID_BEGIN; ui <= _ITEM_ID_END; ui++)
		_lst_wmgr->buttons[ui].bgcolor = 0xff333333;
}

/**
	@Function:		wmgr_window_show
	@Access:		Public
	@Description:
		显示窗体管理器。
	@Parameters:
		x, int32, IN
			窗体管理器显示的X坐标。
		taskbar_y, int32, IN
			任务栏的Y坐标。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
wmgr_window_show(	IN int32 x,
					IN int32 taskbar_y)
{
	_window->x = x;
	_window->y = taskbar_y - _HEIGHT;
	_window->state = WINDOW_STATE_SHOW;
	set_top_window(_window);
	wmgr_window_update(0);
	return TRUE;
}

/**
	@Function:		wmgr_window_hide
	@Access:		Public
	@Description:
		隐藏窗体管理器。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
wmgr_window_hide(void)
{
	_window->state = WINDOW_STATE_HIDDEN;
	return TRUE;
}

/**
	@Function:		_pad
	@Access:		Private
	@Description:
		如果字符串的长度小于_ITEM_MAX_LEN，
		则用空格填充字符串，使其长度等于_ITEM_MAX_LEN。
		如果字符串的长度大于_ITEM_MAX_LEN，
		则截断字符串，使其长度等于_ITEM_MAX_LEN，并且使最后三个字符变为“...”。
	@Parameters:
		text, ASCTEXT, IN OUT
			指向要填充的字符串的缓冲区的指针。
	@Return:
*/
static
void
_pad(IN OUT ASCTEXT text)
{
	int32 slen = (int32)strlen(text);
	if(slen < _ITEM_MAX_LEN)
	{
		int32 len = _ITEM_MAX_LEN - slen;
		int32 i;
		for(i = 0; i < len; i++)
			UtlConcatString(text, _ITEM_MAX_LEN + 1, " ");
	}
	else if(slen > _ITEM_MAX_LEN)
	{
		text[_ITEM_MAX_LEN - 3] = '.';
		text[_ITEM_MAX_LEN - 2] = '.';
		text[_ITEM_MAX_LEN - 1] = '.';
		text[_ITEM_MAX_LEN] = '\0';
	}
}

/**
	@Function:		wmgr_window_update
	@Access:		Public
	@Description:
		更新窗体管理器显示的内容。
	@Parameters:
		offset, uint32, IN
			窗体管理器显示的第1项为窗体数组的第offset项。
			窗体管理器显示的第2项为窗体数组的第offset + 1项。
			以此类推...
	@Return:
*/
void
wmgr_window_update(IN uint32 offset)
{
	uint32 window_count = 0;
	WindowPtr * windows = _get_windows(&window_count);
	uint32 index;
	for(index = _ITEM_ID_BEGIN; index <= _ITEM_ID_END; index++)
	{
		uint32 windex = offset + index - _ITEM_ID_BEGIN;
		if(windex < window_count)
		{
			ASCCHAR title[sizeof(windows[windex]->title)];
			UtlCopyString(title, sizeof(title), windows[windex]->title);
			_pad(title);
			SET_LIST_TEXT(_lst_wmgr, index, title);
			ENABLE_BUTTON(&_lst_wmgr->buttons[index]);
		}
		else
		{
			SET_LIST_TEXT(	_lst_wmgr,
							index,
							"                              ");
			DISABLE_BUTTON(&_lst_wmgr->buttons[index]);
		}
	}
}
