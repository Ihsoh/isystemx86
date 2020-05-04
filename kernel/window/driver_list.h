/**
	@File:			driver_list.h
	@Author:		Ihsoh
	@Date:			2020-05-04
	@Description:
		驱动器列表控件 - Driver List。
*/

#ifndef	_WINDOW_DRIVER_LIST_H_
#define	_WINDOW_DRIVER_LIST_H_

#include "control.h"
#include "list.h"
#include "button.h"

#include "../types.h"
#include "../image.h"
#include "../window.h"

#define	MAX_DRIVER_LIST_ITEM_COUNT		16

typedef	struct
{
	uint32				id;
	int32				type;
	int32				uwid;							// User Window ID。
	int32				uwcid;							// User Window Control ID。
	uint32				x;
	uint32				y;
	List				list;
	
	int8 				symbols[MAX_DRIVER_LIST_ITEM_COUNT][3];
	uint32				count;

	ControlEvent		event;
} DriverList, * DriverListPtr;

#define	INIT_DRIVER_LIST(_driver_list, _x, _y, _event)	\
	(CtrlDriverListInit((_driver_list), 0, (_x), (_y), 0xff000000, 0xff999999, 0xff000000, 0xffbbbbbb, (_event)))

#define	DRIVER_LIST(_driver_list, _image, _params, _top)	\
	(CtrlDriverListUpdate((_driver_list), (_image), (_params), (_top)))

#define	ENABLE_DRIVER_LIST(_driver_list)	\
	(CtrlDriverListEnable((_driver_list)))

#define	DISABLE_DRIVER_LIST(_driver_list)	\
	(CtrlDriverListDisable((_driver_list)))

#define	ENABLE_DRIVER_LIST_ITEM(_driver_list, _index)	\
	(CtrlDriverListEnableItem((_driver_list), (_index)))

#define	DISABLE_DRIVER_LIST_ITEM(_driver_list, _index)	\
	(CtrlDriverListDisableItem((_driver_list), (_index)))

#define	GET_DRIVER_LIST_WIDTH(_driver_list)	\
	(CtrlDriverListGetWidth((_driver_list)))

#define	GET_DRIVER_LIST_HEIGHT(_driver_list)	\
	((_driver_list)->list.count * BUTTON_HEIGHT)

extern
BOOL
CtrlDriverListInit(	OUT DriverListPtr driver_list,
					IN uint32 id,
					IN uint32 x,
					IN uint32 y,
					IN uint32 color,
					IN uint32 bgcolor,
					IN uint32 colorh,
					IN uint32 bgcolorh,
					IN ControlEvent event);

extern
BOOL
CtrlDriverListUpdate(	IN OUT DriverListPtr driver_list,
						OUT ImagePtr image,
						IN WindowEventParamsPtr params,
						BOOL top);

extern
BOOL
CtrlDriverListEnable(IN OUT DriverListPtr driver_list);

extern
BOOL
CtrlDriverListDisable(IN OUT DriverListPtr list);

extern
BOOL
CtrlDriverListEnableItem(	IN OUT DriverListPtr driver_list,
							IN uint32 index);

extern
BOOL
CtrlDriverListDisableItem(	IN OUT DriverListPtr driver_list,
							IN uint32 index);

extern
uint32
CtrlDriverListGetWidth(IN DriverListPtr driver_list);

#endif
