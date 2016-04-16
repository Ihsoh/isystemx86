/**
	@File:			task.c
	@Author:		Ihsoh
	@Date:			2016-01-10
	@Description:
		任务信息窗口。
*/

#include "task.h"

#include "../types.h"
#include "../memory.h"
#include "../tasks.h"

#include <ilib/string.h>

#include "../utils/sfstr.h"

#include "../window/control.h"
#include "../window/button.h"
#include "../window/label.h"

#define	_TITLE			"Task"
#define	_WIDTH			800
#define	_HEIGHT			140

static WindowPtr _window	= NULL;
static ButtonPtr _btn_kill	= NULL;
static LabelPtr _lbl_info	= NULL;

static int32 _tid			= -1;

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
	if(id == _btn_kill->id)
	{
		if(type == BUTTON_LBUP)
		{
			if(_tid != -1 && kill_task(_tid))
			{
				_tid = -1;
				_window->state = WINDOW_STATE_HIDDEN;
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
	switch(params->event_type)
	{
		case WINDOW_EVENT_WILL_CLOSE:
		{
			_window->state = WINDOW_STATE_HIDDEN;
			break;
		}
		case WINDOW_EVENT_PAINT:
		{
			BUTTON(_btn_kill, &window->workspace, params, TRUE);
			LABEL(_lbl_info, &window->workspace, params, TRUE);
			break;
		}
	}
}

/**
	@Function:		task_window_show
	@Access:		Public
	@Description:
		显示任务信息窗口。
	@Parameters:
	@Return:
*/
void
task_window_show(int32 tid)
{
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL)
		return;
	_tid = tid;
	if(_window == NULL)
	{
		_window = create_window(_WIDTH, _HEIGHT,
								0xffffffff,
								WINDOW_STYLE_CLOSE | WINDOW_STYLE_MINIMIZE,
								_TITLE,
								_window_event);
		if(_window == NULL)
			return;
		rect_common_image(&_window->workspace, 0, 0, _WIDTH, _HEIGHT, 0xffffffff);
		_btn_kill = NEW(Button);
		INIT_BUTTON(_btn_kill,
					0,
					0,
					"Kill",
					_control_event);
		_btn_kill->x = 10;
		_btn_kill->y = 10;
		_lbl_info = NEW(Label);
		INIT_LABEL(	_lbl_info,
					0,
					0,
					"Task Infomation",
					_control_event);
		_lbl_info->x = 10;
		_lbl_info->y = 10 + GET_BUTTON_HEIGHT(_btn_kill) + 10;
	}
	if(_window != NULL)
	{
		ASCCHAR buffer[sizeof(_lbl_info->text)];
		UtlCopyString(buffer, sizeof(buffer), "");
		UtlConcatString(buffer, sizeof(buffer), "Name: ");
		UtlConcatString(buffer, sizeof(buffer), task->name);
		UtlConcatString(buffer, sizeof(buffer), "\n");
		UtlConcatString(buffer, sizeof(buffer), "Command: ");
		UtlConcatString(buffer, sizeof(buffer), task->param);
		UtlConcatString(buffer, sizeof(buffer), "\n");
		SET_LABEL_TEXT(_lbl_info, buffer);
		_window->state = WINDOW_STATE_SHOW;
		set_top_window(_window);
	}
}
