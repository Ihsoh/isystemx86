/**
	@File:			list.c
	@Author:		Ihsoh
	@Date:			2015-08-31
	@Description:
		窗体控件 - List。
*/

#include "list.h"
#include "control.h"
#include "button.h"
#include "../types.h"
#include "../image.h"
#include "../enfont.h"
#include "../klib.h"
#include "../window.h"
#include "../kernel.h"

#include <ilib/string.h>

/**
	@Function:		_CtrlListEvent
	@Access:		Private
	@Description:
		List的事件函数。
	@Parameters:
		id, uint32, IN
			控件ID。
		type, uint32, IN
			事件类型。
		params, void *, IN
			指向参数的指针。为NULL时表示没有参数。
	@Return:
*/
static
void
_CtrlListEvent(	IN uint32 id,
				IN uint32 type,
				IN void * param)
{
	if(!IS_CONTROL_ID(id))
	{
		ButtonPtr button = (ButtonPtr)id;
		ListPtr list = (ListPtr)button->vpext;
		if(list == NULL)
			return;
		ControlEvent event = list->event;
		if(event == NULL)
			return;
		uint32 ui;
		for(ui = 0; ui < list->count; ui++)
			if(button == &list->buttons[ui])
			{
				event((uint32)list, type, &ui);
				break;
			}
	}
}

/**
	@Function:		CtrlListInit
	@Access:		Public
	@Description:
		List的事件函数。
	@Parameters:
		list, ListPtr, OUT
			指向List对象的指针。
		id, uint32, IN
			控件ID。
		count, uint32, IN
			List的项的数量。
		x, uint32, IN
			X坐标。
		y, uint32, IN
			Y坐标。
		text, CASCTEXT, IN
			List的项的初始文本。
		color, uint32, IN
			鼠标不在List的项上方时，Button的文本的颜色。
		bgcolor, uint32, IN
			鼠标不在List的项上方时，Button的背景颜色。
		colorh, uint32, IN
			鼠标在List的项上方时，Button的文本的颜色。
		bgcolorh, uint32, IN
			鼠标在List的项上方时，Button的背景颜色。
		event, ControlEvent, IN
			List的事件函数。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlListInit(	OUT ListPtr list,
				IN uint32 id,
				IN uint32 count,
				IN uint32 x,
				IN uint32 y,
				IN CASCTEXT text,
				IN uint32 color,
				IN uint32 bgcolor,
				IN uint32 colorh,
				IN uint32 bgcolorh,
				IN ControlEvent event)
{
	if(	list == NULL
		|| count > MAX_LIST_ITEM_COUNT
		|| text == NULL)
		return FALSE;
	if(id == 0)
		list->id = (uint32)list;
	else
		list->id = id;
	list->type = CONTROL_LIST;
	list->uwid = -1;
	list->uwcid = -1;
	list->x = x;
	list->y = y;
	list->count = count;
	list->event = event;
	uint32 ui;
	for(ui = 0; ui < count; ui++)
	{
		ButtonPtr button = &list->buttons[ui];
		CtrlBtnInit(button,
					(uint32)button,
					x, y + ui * BUTTON_HEIGHT,
					text,
					color, bgcolor, colorh, bgcolorh,
					_CtrlListEvent,
					0, 0);
		button->vpext = list;
	}
	return TRUE;
}

/**
	@Function:		CtrlListUpdate
	@Access:		Public
	@Description:
		List的渲染、事件处理函数。
	@Parameters:
		list, ListPtr, IN OUT
			指向List对象的指针。
		image, ImagePtr, OUT
			指向目标图片对象的指针。
		params, WindowEventParamsPtr, IN
			指向窗口事件参数的指针。
		top, BOOL, IN
			指示List所在的窗体是否在最顶层。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlListUpdate(	IN OUT ListPtr list,
				OUT ImagePtr image,
				IN WindowEventParamsPtr params,
				BOOL top)
{
	if(list == NULL || image == NULL || params == NULL)
		return FALSE;
	if(!top)
		return TRUE;
	uint32 ui;
	for(ui = 0; ui < list->count; ui++)
		if(!CtrlBtnUpdate(&list->buttons[ui], image, params, top))
			return FALSE;
	return TRUE;
}

/**
	@Function:		CtrlListSetText
	@Access:		Public
	@Description:
		设置List的项的文本。
	@Parameters:
		list, ListPtr, IN OUT
			指向List对象的指针。
		index, uint32, IN
			项的索引。
		text, CASCTEXT, IN
			需要设置的文本。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlListSetText(IN OUT ListPtr list,
				IN uint32 index,
				IN CASCTEXT text)
{
	if(	list == NULL
		|| index >= list->count
		|| text == NULL)
		return FALSE;
	ButtonPtr button = &list->buttons[index];
	return CtrlBtnSetText(button, text);
}

/**
	@Function:		CtrlListEnable
	@Access:		Public
	@Description:
		启用List。
	@Parameters:
		list, ListPtr, IN OUT
			指向List对象的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlListEnable(IN OUT ListPtr list)
{
	if(list == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < list->count; ui++)
		ENABLE_BUTTON(&list->buttons[ui]);
	return TRUE;
}

/**
	@Function:		CtrlListDisable
	@Access:		Public
	@Description:
		禁用List。
	@Parameters:
		list, ListPtr, IN OUT
			指向List对象的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlListDisable(IN OUT ListPtr list)
{
	if(list == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < list->count; ui++)
		DISABLE_BUTTON(&list->buttons[ui]);
	return TRUE;
}

/**
	@Function:		CtrlListEnableItem
	@Access:		Public
	@Description:
		启用列表项。
	@Parameters:
		list, ListPtr, IN OUT
			指向List对象的指针。
		index, uint32, IN
			索引。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlListEnableItem(	IN OUT ListPtr list,
					IN uint32 index)
{
	if(	list == NULL
		|| index >= list->count)
		return FALSE;
	ENABLE_BUTTON(&list->buttons[index]);
	return TRUE;
}

/**
	@Function:		CtrlListDisableItem
	@Access:		Public
	@Description:
		禁用列表项。
	@Parameters:
		list, ListPtr, IN OUT
			指向List对象的指针。
		index, uint32, IN
			索引。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlListDisableItem(IN OUT ListPtr list,
					IN uint32 index)
{
	if(	list == NULL
		|| index >= list->count)
		return FALSE;
	DISABLE_BUTTON(&list->buttons[index]);
	return TRUE;
}

/**
	@Function:		CtrlListGetWidth
	@Access:		Public
	@Description:
		获取列表的宽度。
	@Parameters:
		list, ListPtr, IN
			指向List对象的指针。
	@Return:
		uint32
			获取列表的宽度。
*/
uint32
CtrlListGetWidth(IN ListPtr list)
{
	if(list == NULL)
		return 0;
	uint32 max_width = 0;
	uint32 ui;
	for(ui = 0; ui < list->count; ui++)
	{
		uint32 width = GET_BUTTON_WIDTH(&list->buttons[ui]);
		if(width > max_width)
			max_width = width;
	}
	return max_width;
}
