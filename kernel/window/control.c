/**
	@File:			control.c
	@Author:		Ihsoh
	@Date:			2015-08-31
	@Description:
		窗体控件。
*/

#include "control.h"

#include "button.h"
#include "label.h"
#include "list.h"
#include "progress.h"
#include "scroll.h"
#include "edit.h"

#include "../types.h"

static int32 _count = 0;

void
__dummy_event(uint32 id, uint32 type, void * param)
{
}

int32
__new_control_id(void)
{
	return _count++;
}

/**
	@Function:		control_get_width
	@Access:		Public
	@Description:
		获取控件宽度。
	@Parameters:
		c, void *, IN
			指向控件对象的指针。
	@Return:
		uint32
			控件的宽度。如果失败会为0。
*/
uint32
control_get_width(IN void * c)
{
	if(c == NULL)
		return 0;
	ControlPtr control = (ControlPtr)c;
	switch(control->type)
	{
		case CONTROL_BUTTON:
			return GET_BUTTON_WIDTH((ButtonPtr)control);
		case CONTROL_LABEL:
			return GET_LABEL_WIDTH((LabelPtr)control);
		case CONTROL_LIST:
			return GET_LIST_WIDTH((ListPtr)control);
		case CONTROL_PROGRESS:
			return GET_PROGRESS_WIDTH((ProgressPtr)control);
		case CONTROL_SCROLL:
			return GET_SCROLL_WIDTH((ScrollPtr)control);
		case CONTROL_EDIT:
			return GET_EDIT_WIDTH((EditPtr)control);
		default:
			return 0;
	}
}

/**
	@Function:		control_get_height
	@Access:		Public
	@Description:
		获取控件高度。
	@Parameters:
		c, void *, IN
			指向控件对象的指针。
	@Return:
		uint32
			控件的高度。如果失败会为0。
*/
uint32
control_get_height(IN void * c)
{
	if(c == NULL)
		return 0;
	ControlPtr control = (ControlPtr)c;
	switch(control->type)
	{
		case CONTROL_BUTTON:
			return GET_BUTTON_HEIGHT((ButtonPtr)control);
		case CONTROL_LABEL:
			return GET_LABEL_HEIGHT((LabelPtr)control);
		case CONTROL_LIST:
			return GET_LIST_HEIGHT((ListPtr)control);
		case CONTROL_PROGRESS:
			return GET_PROGRESS_HEIGHT((ProgressPtr)control);
		case CONTROL_SCROLL:
			return GET_SCROLL_HEIGHT((ScrollPtr)control);
		case CONTROL_EDIT:
			return GET_EDIT_HEIGHT((EditPtr)control);
		default:
			return 0;
	}
}
