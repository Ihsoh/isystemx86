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

#include "../window/button.h"

#include <ilib/string.h>

#define	_WIDTH	640
#define	_HEIGHT	480

static struct Window * _window = NULL;

static
void
_f(int32 id, uint32 type, void * param)
{
	if(id == 1)
	{
		if(type == BUTTON_LBUP)
			print_str("<BUTTON1 UP>\n");
		else if(type == BUTTON_LBDOWN)
			print_str("<BUTTON1 DOWN>\n");
		else if(type == BUTTON_IN)
			print_str("<BUTTON1 IN>\n");
		else if(type == BUTTON_OUT)
			print_str("<BUTTON1 OUT>\n");
	}
	else if(id == 2)
	{
		if(type == BUTTON_RBUP)
			print_str("<BUTTON2 UP>\n");
		else if(type == BUTTON_RBDOWN)
			print_str("<BUTTON2 DOWN>\n");
	}
}

Button btn1, btn2;

static
void
_window_event(	IN struct Window * window,
				IN struct WindowEventParams * params)
{
	BOOL top = get_top_window() == window;
	BUTTON(&btn1, &window->workspace, params, top);
	BUTTON(&btn2, &window->workspace, params, top);
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
	INIT_BUTTON(&btn1, 1, 10, 10, "Test1", _f);
	INIT_BUTTON(&btn2, 2, 100, 10, "Test2", _f);
	btn2.width = 200;
	return TRUE;
}

void
explorer_window_show(void)
{
	_window->state = WINDOW_STATE_SHOW;
}
