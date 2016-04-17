/**
	@File:			dbgout.c
	@Author:		Ihsoh
	@Date:			2016-01-01
	@Description:
		调试输出窗体。
*/

#include "dbgout.h"
#include "../types.h"
#include "../memory.h"
#include "../window.h"

#include "../window/control.h"
#include "../window/edit.h"


#define	_TITLE		"Debug Output"
#define	_TEXT		"ISystem Debug Output Window\n"

#define	_COLUMN		80
#define	_ROW		6

static WindowPtr _window	= NULL;
static EditPtr _edt_dbgout	= NULL;

/**
	@Function:		WinDbgoutEvent
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
WinDbgoutEvent(	IN struct Window * window,
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
			NMLEDIT(_edt_dbgout, &window->workspace, params, top);
			break;
		}
	}
}

/**
	@Function:		WinDbgoutInit
	@Access:		Public
	@Description:
		初始化调试输出窗体。
	@Parameters:
	@Return:
*/
void
WinDbgoutInit(void)
{
	_edt_dbgout = NEW(Edit);
	if(!INIT_EDIT(	_edt_dbgout,
					0, 0,
					_ROW, _COLUMN,
					EDIT_STYLE_REFRESH 
						| EDIT_STYLE_READONLY,
					NULL))
		goto err;
	SET_EDIT_TEXT(_edt_dbgout, _TEXT);
	_edt_dbgout->x = 0;
	_edt_dbgout->y = 0;
	uint32 w = GET_EDIT_WIDTH(_edt_dbgout);
	uint32 h = GET_EDIT_HEIGHT(_edt_dbgout);
	_window = create_window(w, h,
							0xffffffff,
							WINDOW_STYLE_CLOSE
								| WINDOW_STYLE_MINIMIZE
								| WINDOW_STYLE_NO_WMGR,
							_TITLE,
							WinDbgoutEvent);
	if(_window == NULL)
		goto err;
	return;
err:
	if(_window != NULL)
	{
		destroy_window(_window);
		_window = NULL;
	}
	if(_edt_dbgout != NULL)
	{
		UNINIT_EDIT(_edt_dbgout);
		DELETE(_edt_dbgout);
		_edt_dbgout = NULL;
	}
}

/**
	@Function:		WinDbgoutShow
	@Access:		Public
	@Description:
		显示调试输出窗体。
	@Parameters:
	@Return:
*/
void
WinDbgoutShow(void)
{
	if(_window == NULL)
		return;
	_window->state = WINDOW_STATE_SHOW;
}

/**
	@Function:		WinDbgoutOutput
	@Access:		Public
	@Description:
		把文本输出到调试输出窗体的文本框。
	@Parameters:
		text, CASCTEXT, IN
			指向储存文本的缓冲区的指针。
	@Return:
*/
void
WinDbgoutOutput(IN CASCTEXT text)
{
	if(	_edt_dbgout == NULL
		|| _window == NULL)
		return;
	APPEND_EDIT_TEXT(_edt_dbgout, text);
}
