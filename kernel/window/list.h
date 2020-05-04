/**
	@File:			list.h
	@Author:		Ihsoh
	@Date:			2015-08-31
	@Description:
		窗体控件 - List。
*/

#ifndef	_WINDOW_LIST_H_
#define	_WINDOW_LIST_H_

#include "control.h"
#include "button.h"

#include "../types.h"
#include "../image.h"
#include "../window.h"

#define	MAX_LIST_ITEM_COUNT		1024

typedef	struct
{
	uint32				id;
	int32				type;
	int32				uwid;							// User Window ID。
	int32				uwcid;							// User Window Control ID。
	uint32				x;
	uint32				y;
	Button 				buttons[MAX_LIST_ITEM_COUNT];
	uint32				count;

	void *				vpext;

	ControlEvent		event;
} List, * ListPtr;

#define	INIT_LIST(_list, _count, _x, _y, _text, _event)	\
	(CtrlListInit((_list), 0, (_count), (_x), (_y), (_text), 0xff000000, 0xff999999, 0xff000000, 0xffbbbbbb, (_event)))

#define	LIST(_list, _image, _params, _top)	\
	(CtrlListUpdate((_list), (_image), (_params), (_top)))

#define	SET_LIST_TEXT(_list, _index, _text)	\
	(CtrlListSetText((_list), (_index), (_text)))

#define	ENABLE_LIST(_list)	\
	(CtrlListEnable((_list)))

#define	DISABLE_LIST(_list)	\
	(CtrlListDisable((_list)))

#define	ENABLE_LIST_ITEM(_list, _index)	\
	(CtrlListEnableItem((_list), (_index)))

#define	DISABLE_LIST_ITEM(_list, _index)	\
	(CtrlListDisableItem((_list), (_index)))

#define	GET_LIST_WIDTH(_list)	\
	(CtrlListGetWidth((_list)))

#define	GET_LIST_HEIGHT(_list)	\
	((_list)->count * BUTTON_HEIGHT)

extern
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
				IN ControlEvent event);

extern
BOOL
CtrlListUpdate(	IN OUT ListPtr list,
				OUT ImagePtr image,
				IN WindowEventParamsPtr params,
				BOOL top);

extern
BOOL
CtrlListSetText(IN OUT ListPtr list,
				IN uint32 index,
				IN CASCTEXT text);

extern
BOOL
CtrlListEnable(IN OUT ListPtr list);

extern
BOOL
CtrlListDisable(IN OUT ListPtr list);

extern
BOOL
CtrlListEnableItem(	IN OUT ListPtr list,
					IN uint32 index);

extern
BOOL
CtrlListDisableItem(IN OUT ListPtr list,
					IN uint32 index);

extern
uint32
CtrlListGetWidth(IN ListPtr list);

#endif
