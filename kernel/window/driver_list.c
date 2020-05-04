/**
	@File:			driver_list.c
	@Author:		Ihsoh
	@Date:			2020-05-04
	@Description:
		驱动器列表控件 - Driver List。
*/

#include "driver_list.h"
#include "control.h"
#include "list.h"
#include "../types.h"
#include "../image.h"
#include "../enfont.h"
#include "../klib.h"
#include "../window.h"
#include "../kernel.h"
#include "../disk.h"

#include "../utils/sfstr.h"

#include <ilib/string.h>

/**
	@Function:		_Event
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
_Event(	IN uint32 id,
		IN uint32 type,
		IN void * param)
{
	if(!IS_CONTROL_ID(id))
	{
		ListPtr list = (ListPtr)id;
		DriverListPtr driver_list = (DriverListPtr)list->vpext;
		if(driver_list == NULL)
			return;
		ControlEvent event = driver_list->event;
		if(event == NULL)
			return;
		uint32 list_item_index = *(uint32*)param;
		event((uint32)driver_list, type, driver_list->symbols[list_item_index]);
	}
}

/**
	@Function:		CtrlDriverListInit
	@Access:		Public
	@Description:
		DriverList的初始化函数。
	@Parameters:
		driver_list, DriverListPtr, OUT
			指向DriverList对象的指针。
		id, uint32, IN
			控件ID。
		x, uint32, IN
			X坐标。
		y, uint32, IN
			Y坐标。
		color, uint32, IN
			鼠标不在DriverList的项上方时，Button的文本的颜色。
		bgcolor, uint32, IN
			鼠标不在DriverList的项上方时，Button的背景颜色。
		colorh, uint32, IN
			鼠标在DriverList的项上方时，Button的文本的颜色。
		bgcolorh, uint32, IN
			鼠标在DriverList的项上方时，Button的背景颜色。
		event, ControlEvent, IN
			DriverList的事件函数。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlDriverListInit(	OUT DriverListPtr driver_list,
					IN uint32 id,
					IN uint32 x,
					IN uint32 y,
					IN uint32 color,
					IN uint32 bgcolor,
					IN uint32 colorh,
					IN uint32 bgcolorh,
					IN ControlEvent event)
{
	if(driver_list == NULL)
		return FALSE;
	if(id == 0)
		driver_list->id = (uint32)driver_list;
	else
		driver_list->id = id;
	driver_list->type = CONTROL_DRIVER_LIST;
	driver_list->uwid = -1;
	driver_list->uwcid = -1;
	driver_list->x = x;
	driver_list->y = y;
	driver_list->event = event;

	uint32 count = DskGetCount();
	driver_list->list.count = count;
	driver_list->count = count;

	CtrlListInit(
		&(driver_list->list),
		&(driver_list->list),
		count,
		x, y,
		"###",
		color, bgcolor,
		colorh, bgcolorh,
		_Event
	);

	driver_list->list.vpext = driver_list;

	uint32 i;
	for(i = 0; i < count && i < MAX_DRIVER_LIST_ITEM_COUNT; i++)
	{
		int8 symbol[3];
		DskGetBySymbol(i, symbol);

		UtlCopyString(
			driver_list->symbols[i],
			sizeof(driver_list->symbols[i]),
			symbol
		);

		CtrlListSetText(&(driver_list->list), i, symbol);
	}
}

/**
	@Function:		CtrlDriverListUpdate
	@Access:		Public
	@Description:
		DriverList的渲染、事件处理函数。
	@Parameters:
		driver_list, DriverListPtr, IN OUT
			指向DriverList对象的指针。
		image, ImagePtr, OUT
			指向目标图片对象的指针。
		params, WindowEventParamsPtr, IN
			指向窗口事件参数的指针。
		top, BOOL, IN
			指示DriverList所在的窗体是否在最顶层。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlDriverListUpdate(	IN OUT DriverListPtr driver_list,
						OUT ImagePtr image,
						IN WindowEventParamsPtr params,
						BOOL top)
{
	if(driver_list == NULL || image == NULL || params == NULL)
		return FALSE;
	if(!top)
		return TRUE;
	return LIST(&(driver_list->list), image, params, top);
}

/**
	@Function:		CtrlDriverListEnable
	@Access:		Public
	@Description:
		启用DriverList。
	@Parameters:
		driver_list, DriverListPtr, IN OUT
			指向DriverList对象的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlDriverListEnable(IN OUT DriverListPtr driver_list)
{
	if(driver_list == NULL)
		return FALSE;
	return CtrlListEnable(&(driver_list->list));
}

/**
	@Function:		CtrlDriverListDisable
	@Access:		Public
	@Description:
		禁用DriverList。
	@Parameters:
		driver_list, DriverListPtr, IN OUT
			指向DriverList对象的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlDriverListDisable(IN OUT DriverListPtr driver_list)
{
	if(driver_list == NULL)
		return FALSE;
	return CtrlListDisable(&(driver_list->list));
}

/**
	@Function:		CtrlDriverListEnableItem
	@Access:		Public
	@Description:
		启用列表项。
	@Parameters:
		driver_list, DriverListPtr, IN OUT
			指向DriverList对象的指针。
		index, uint32, IN
			索引。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlDriverListEnableItem(	IN OUT DriverListPtr driver_list,
							IN uint32 index)
{
	if(driver_list == NULL)
		return FALSE;
	return CtrlListEnableItem(&(driver_list->list), index);
}

/**
	@Function:		CtrlDriverListDisableItem
	@Access:		Public
	@Description:
		禁用列表项。
	@Parameters:
		driver_list, DriverListPtr, IN OUT
			指向DriverList对象的指针。
		index, uint32, IN
			索引。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
CtrlDriverListDisableItem(	IN OUT DriverListPtr driver_list,
							IN uint32 index)
{
	if(driver_list == NULL)
		return FALSE;
	return CtrlListDisableItem(&(driver_list->list), index);
}

/**
	@Function:		CtrlDriverListGetWidth
	@Access:		Public
	@Description:
		获取列表的宽度。
	@Parameters:
		driver_list, DriverListPtr, IN
			指向DriverList对象的指针。
	@Return:
		uint32
			获取列表的宽度。
*/
uint32
CtrlDriverListGetWidth(IN DriverListPtr driver_list)
{
	if(driver_list == NULL)
		return FALSE;
	return CtrlListGetWidth(&(driver_list->list));
}
