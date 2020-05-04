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

#include "../utils/sfstr.h"

#include "../window/button.h"
#include "../window/label.h"
#include "../window/list.h"
#include "../window/progress.h"
#include "../window/scroll.h"
#include "../window/driver_list.h"
#include "../window/file_list.h"

#include <ilib/string.h>

#define	_WIDTH	640
#define	_HEIGHT	640

static struct Window * _window = NULL;

ButtonPtr btn1 = NULL;
ButtonPtr btn2 = NULL;
LabelPtr lbl1 = NULL;
ListPtr lst1 = NULL;
ProgressPtr prgr1 = NULL;
ScrollPtr scrl1 = NULL;
DriverListPtr drvlst1 = NULL;
FileListPtr filelst1 = NULL;

static
void
_F(uint32 id, uint32 type, void * param)
{
	if(id == btn1->id)
	{
		if(type == BUTTON_LBUP)
		{
			SET_LABEL_TEXT(lbl1, "<BUTTON1 UP>\n");
			
			if (filelst1->top > 0)
			{
				CtrlFileListSetTop(filelst1, filelst1->top - 1);
			}
		}
		else if(type == BUTTON_LBDOWN)
			SET_LABEL_TEXT(lbl1, "<BUTTON1 DOWN>\n");
	}
	else if(id == btn2->id)
	{
		if(type == BUTTON_LBUP)
		{
			SET_LABEL_TEXT(lbl1, "<BUTTON2 UP>\n");

			if (filelst1->top + 1 < filelst1->count)
			{
				CtrlFileListSetTop(filelst1, filelst1->top + 1);
			}
		}
		else if(type == BUTTON_LBDOWN)
			SET_LABEL_TEXT(lbl1, "<BUTTON2 DOWN>\n");
	}
	else if(id == lst1->id)
	{
		ASCCHAR buffer[1024];
		if(type == BUTTON_LBUP)
		{
			SET_PROGRESS_PERCENT(prgr1, *(uint32 *)param * 10);
			SET_LABEL_TEXT(lbl1, uitos(buffer, *(uint32 *)param));
		}
	}
	else if(id == scrl1->id)
	{
		ASCCHAR buffer[1024];
		if(type == SCROLL_CHANGED)
		{
			SET_PROGRESS_PERCENT(prgr1, scrl1->value);
			SET_LABEL_TEXT(lbl1, uitos(buffer, scrl1->value));
		}
	}
	else if(id == drvlst1->id)
	{
		if(type == BUTTON_LBUP)
		{
			ASCCHAR buffer[1024];
			UtlCopyString(buffer, sizeof(buffer), param);
			SET_LABEL_TEXT(lbl1, buffer);
		}
	}
	else if(id == filelst1->id)
	{
		if(type == BUTTON_LBUP)
		{
			FileListItemPtr file_list_item = (FileListItemPtr)param;

			SET_LABEL_TEXT(lbl1, file_list_item->name);
		}
	}
}

static
void
_WinExplEvent(	IN struct Window * window,
				IN struct WindowEventParams * params)
{
	BOOL top = ScrGetTopWindow() == window;
	if(params->event_type == WINDOW_EVENT_PAINT)
	{
		BUTTON(btn1, &window->workspace, params, top);
		BUTTON(btn2, &window->workspace, params, top);
		LABEL(lbl1, &window->workspace, params, top);
		LIST(lst1, &window->workspace, params, top);
		PROGRESS(prgr1, &window->workspace, params, top);
		SCROLL(scrl1, &window->workspace, params, top);
		DRIVER_LIST(drvlst1, &window->workspace, params, top);

		CtrlFileListUpdate(filelst1, &window->workspace, params, top);
	}
}

BOOL
WinExplInit(void)
{
	_window = ScrCreateWindow(_WIDTH,
							_HEIGHT,
							0xffffffff,
							WINDOW_STYLE_MINIMIZE,
							"Explorer",
							_WinExplEvent);
	rect_common_image(&_window->workspace, 0, 0, _WIDTH, _HEIGHT, 0xffffffff);
	if(_window == NULL)
		return FALSE;
	btn1 = NEW(Button);
	btn2 = NEW(Button);
	lbl1 = NEW(Label);
	lst1 = NEW(List);
	prgr1 = NEW(Progress);
	scrl1 = NEW(Scroll);
	drvlst1 = NEW(DriverList);
	filelst1 = NEW(FileList);
	INIT_BUTTON(btn1, 10, 10, "Test1", _F);
	INIT_BUTTON(btn2, 100, 10, "Test2", _F);
	INIT_LABEL(lbl1, 10, 100, "This\nis\nTest Label!", _F);
	INIT_LIST(	lst1,
				5,
				10, 200,
				"#----------#",
				_F);
	INIT_PROGRESS(prgr1, 10, 350, 300, 40, 50, _F);
	INIT_SCROLL(scrl1, 10, 400, 300, 30, 10, 0, 100, _F);
	INIT_DRIVER_LIST(drvlst1, 10, 450, _F);

	CtrlFileListInit(
		filelst1,
		0,
		5,
		200, 450,
		0xff000000, 0xff999999, 0xff000000, 0xffbbbbbb,
		_F
	);
	CtrlFileListSetPath(filelst1, "SA:/isystem/");
	CtrlFileListSetTop(filelst1, 0);

	return TRUE;
}

void
WinExplShow(void)
{
	_window->state = WINDOW_STATE_SHOW;
}
