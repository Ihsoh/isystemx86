/**
	@File:			taskmgr.c
	@Author:		Ihsoh
	@Date:			2016-01-09
	@Description:
		任务管理器窗口。
*/

#include "taskmgr.h"
#include "task.h"

#include "../types.h"
#include "../memory.h"
#include "../tasks.h"

#include <ilib/string.h>

#include "../window/control.h"
#include "../window/button.h"
#include "../window/list.h"

#define	_TITLE			"Task Manager"

#define	_ITEM_COUNT		20
#define	_ITEM_MAX_LEN	60

#define	_ITEM_ID_UP		0
#define	_ITEM_ID_BEGIN	(_ITEM_ID_UP + 1)
#define	_ITEM_ID_END	(_ITEM_COUNT - 3)
#define	_ITEM_ID_DOWN	(_ITEM_COUNT - 2)
#define	_ITEM_ID_CLOSE	(_ITEM_COUNT - 1)

#define	_TITLE_COUNT	(_ITEM_ID_END - _ITEM_ID_BEGIN + 1)

#define	_WIDTH		(BUTTON_LPADDING + _ITEM_MAX_LEN * ENFONT_WIDTH + BUTTON_RPADDING)
#define	_HEIGHT		(24 * _ITEM_COUNT)

static WindowPtr _window	= NULL;

static ListPtr _lst_taskmgr	= NULL;

static uint32 _offset		= 0;

/**
	@Function:		_WinTskmgrGetTasks
	@Access:		Private
	@Description:
		获取所有任务的ID。
	@Parameters:
		count, uint32 *, OUT
			指向储存任务数量的缓冲区的指针。
	@Return:
		int32 *
			指向储存所有任务的ID的缓冲区的指针。
*/
static
int32 *
_WinTskmgrGetTasks(OUT uint32 * count)
{
	static int32 tasks[MAX_TASK_COUNT];
	if(count == NULL)
		return NULL;
	int32 i;
	for(i = 0; i < MAX_TASK_COUNT; i++)
		if(get_task_info_ptr(i) != NULL)
			tasks[(*count)++] = i;
	return tasks;
}

/**
	@Function:		_WinTskmgrControlEvent
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
_WinTskmgrControlEvent(	IN uint32 id,
						IN uint32 type,
						IN void * param)
{
	if(id == _lst_taskmgr->id)
	{
		if(type == BUTTON_LBUP)
		{
			uint32 id = *(uint32 *)param;
			switch(id)
			{
				case _ITEM_ID_UP:
					if(_offset > 0)
						_offset--;
					WinTskmgrUpdate(_offset);
					break;
				case _ITEM_ID_DOWN:
				{
					uint32 task_count = 0;
					_WinTskmgrGetTasks(&task_count);
					if(_offset + _TITLE_COUNT < task_count)
						_offset++;
					WinTskmgrUpdate(_offset);
					break;
				}
				case _ITEM_ID_CLOSE:
					WinTskmgrHide();
					break;
				default:
					if(id >= _ITEM_ID_BEGIN && id <= _ITEM_ID_END)
					{
						uint32 tindex = _offset + id - _ITEM_ID_BEGIN;
						uint32 task_count = 0;
						int32 * tasks = _WinTskmgrGetTasks(&task_count);
						if(tindex < task_count)
							WinTaskShow(tasks[tindex]);
					}
					break;
			}
		}
	}
}

/**
	@Function:		_WinTskmgrEvent
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
_WinTskmgrEvent(IN struct Window * window,
				IN struct WindowEventParams * params)
{
	BOOL top = get_top_window() == window;
	switch(params->event_type)
	{
		case WINDOW_EVENT_PAINT:
		{
			static int32 counter = 0;
			if(++counter == 20)
			{
				WinTskmgrUpdate(_offset);
				counter = 0;
			}
			LIST(_lst_taskmgr, &window->workspace, params, TRUE);
			break;
		}
		case WINDOW_EVENT_WILL_CLOSE:
		{
			WinTskmgrHide();
			break;
		}
	}
}

/**
	@Function:		_WinTskmgrRPadString
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
_WinTskmgrRPadString(IN OUT ASCTEXT text)
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
	@Function:		WinTskmgrInit
	@Access:		Public
	@Description:
		初始化任务管理器窗口。
	@Parameters:
	@Return:
*/
void
WinTskmgrInit(void)
{
	_window = create_window(_WIDTH, _HEIGHT,
							0xff222222,
							WINDOW_STYLE_CLOSE
								| WINDOW_STYLE_MINIMIZE
								| WINDOW_STYLE_NO_WMGR,
							_TITLE,
							_WinTskmgrEvent);
	rect_common_image(&_window->workspace, 0, 0, _WIDTH, _HEIGHT, 0xffffffff);
	_lst_taskmgr = NEW(List);
	CtrlListInit(	_lst_taskmgr, 0,
				_ITEM_COUNT,
				0, 0,
				"",
				0xffffffff, 0xff222222, 0xffffffff, 0xff444444,
				_WinTskmgrControlEvent);
	ASCCHAR text[MAX_BUTTON_TEXT_LEN + 1];
	UtlCopyString(text, sizeof(text), "Up");
	_WinTskmgrRPadString(text);
	SET_LIST_TEXT(_lst_taskmgr, _ITEM_ID_UP, text);
	UtlCopyString(text, sizeof(text), "Down");
	_WinTskmgrRPadString(text);
	SET_LIST_TEXT(_lst_taskmgr, _ITEM_ID_DOWN, text);
	UtlCopyString(text, sizeof(text), "Close");
	_WinTskmgrRPadString(text);
	SET_LIST_TEXT(_lst_taskmgr, _ITEM_ID_CLOSE, text);
	uint32 ui;
	for(ui = 0; ui < _lst_taskmgr->count; ui++)
		_lst_taskmgr->buttons[ui].style = BUTTON_STYLE_REFRESH;
	for(ui = _ITEM_ID_BEGIN; ui <= _ITEM_ID_END; ui++)
		_lst_taskmgr->buttons[ui].bgcolor = 0xff333333;
	WinTskmgrUpdate(_offset);
}

/**
	@Function:		WinTskmgrShow
	@Access:		Public
	@Description:
		显示任务管理器窗口。
	@Parameters:
	@Return:
*/
void
WinTskmgrShow(void)
{
	if(_window == NULL)
		return;
	_window->state = WINDOW_STATE_SHOW;
	set_top_window(_window);
	WinWmgrUpdate(0);
}

/**
	@Function:		WinTskmgrHide
	@Access:		Public
	@Description:
		隐藏任务管理器窗口。
	@Parameters:
	@Return:
*/
void
WinTskmgrHide(void)
{
	_window->state = WINDOW_STATE_HIDDEN;
}

/**
	@Function:		WinTskmgrUpdate
	@Access:		Public
	@Description:
		更新任务管理器窗口显示的内容。
	@Parameters:
		offset, uint32, IN
			任务管理器窗口显示的第1项为任务数组的第offset项的值所代表的任务的信息。
			任务管理器窗口显示的第2项为任务数组的第offset + 1项的值所代表的任务的信息。
			以此类推...
	@Return:
*/
void
WinTskmgrUpdate(IN uint32 offset)
{
	uint32 task_count = 0;
	int32 * tasks = _WinTskmgrGetTasks(&task_count);
	uint32 index;
	for(index = _ITEM_ID_BEGIN; index <= _ITEM_ID_END; index++)
	{
		uint32 tindex = offset + index - _ITEM_ID_BEGIN;
		if(tindex < task_count)
		{
			ASCCHAR title[MAX_BUTTON_TEXT_LEN + 1];
			int32 tid = tasks[tindex];
			itos(title, tid);
			UtlConcatString(title, sizeof(title), "# ");
			struct Task * task = get_task_info_ptr(tid);
			if(task != NULL)
			{
				UtlConcatString(title, sizeof(title), task->name);
				_WinTskmgrRPadString(title);
			}
			SET_LIST_TEXT(_lst_taskmgr, index, title);
			ENABLE_BUTTON(&_lst_taskmgr->buttons[index]);
		}
		else
		{
			ASCCHAR title[MAX_BUTTON_TEXT_LEN + 1];
			title[0] = '\0';
			_WinTskmgrRPadString(title);
			SET_LIST_TEXT(	_lst_taskmgr,
							index,
							title);
			DISABLE_BUTTON(&_lst_taskmgr->buttons[index]);
		}
	}
}
