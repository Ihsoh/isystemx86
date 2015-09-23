/**
	@File:			explorer.c
	@Author:		Ihsoh
	@Date:			2015-08-30
	@Description:
		文件管理器。
*/

#include "explorer.h"
#include "../types.h"
#include "../enfont.h"
#include "../window.h"
#include "../image.h"
#include "../klib.h"
#include "../memory.h"

#include "../window/button.h"
#include "../window/label.h"
#include "../window/list.h"
#include "../window/progress.h"

#include <ilib/string.h>

#define	_WIDTH	640
#define	_HEIGHT	480

static struct Window * _window = NULL;

ButtonPtr btn1 = NULL;
ButtonPtr btn2 = NULL;
LabelPtr lbl1 = NULL;
ListPtr lst1 = NULL;
ProgressPtr prgr1 = NULL;

static
void
_f(uint32 id, uint32 type, void * param)
{
	if(id == btn1->id)
	{
		if(type == BUTTON_LBUP)
			SET_LABEL_TEXT(lbl1, "<BUTTON1 UP>\n");
		else if(type == BUTTON_LBDOWN)
			SET_LABEL_TEXT(lbl1, "<BUTTON1 DOWN>\n");
		btn2->enabled = FALSE;
	}
	else if(id == btn2->id)
	{
		if(type == BUTTON_RBUP)
			SET_LABEL_TEXT(lbl1, "<BUTTON2 UP>\n");
		else if(type == BUTTON_RBDOWN)
			SET_LABEL_TEXT(lbl1, "<BUTTON2 DOWN>\n");
	}
	else if(id == lst1->id)
	{
		ASCCHAR buffer[1024];
		if(type == BUTTON_LBUP)
		{
			progress_set_percent(prgr1, *(uint32 *)param * 10);
			SET_LABEL_TEXT(lbl1, uitos(buffer, *(uint32 *)param));
		}
	}
}

static
void
_window_event(	IN struct Window * window,
				IN struct WindowEventParams * params)
{
	BOOL top = get_top_window() == window;
	if(params->event_type == WINDOW_EVENT_PAINT)
	{
		BUTTON(btn1, &window->workspace, params, top);
		BUTTON(btn2, &window->workspace, params, top);
		LABEL(lbl1, &window->workspace, params, top);
		LIST(lst1, &window->workspace, params, top);
		PROGRESS(prgr1, &window->workspace, params, top);
	}
}

BOOL
explorer_window_init(void)
{
	_window = create_window(_WIDTH,
							_HEIGHT,
							0xffffffff,
							WINDOW_STYLE_MINIMIZE,
							"Explorer",
							_window_event);
	rect_common_image(&_window->workspace, 0, 0, _WIDTH, _HEIGHT, 0xffffffff);
	if(_window == NULL)
		return FALSE;
	btn1 = NEW(Button);
	btn2 = NEW(Button);
	lbl1 = NEW(Label);
	lst1 = NEW(List);
	prgr1 = NEW(Progress);
	INIT_BUTTON(btn1, 10, 10, "Test1", _f);
	INIT_BUTTON(btn2, 100, 10, "Test2", _f);
	INIT_LABEL(lbl1, 10, 100, "This\nis\nTest Label!", _f);
	INIT_LIST(	lst1,
				5,
				10, 200,
				"#----------#",
				_f);
	progress_init(	prgr1,
					0,
					50,
					10, 350,
					300, 40,
					0xffaaaaaa, 0xff555555,
					_f);
	return TRUE;
}

void
explorer_window_show(void)
{
	_window->state = WINDOW_STATE_SHOW;
}
