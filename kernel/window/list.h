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

#define	MAX_LIST_ITEM_COUNT		256

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
	ControlEvent		event;
} List, * ListPtr;

#define	INIT_LIST(_list, _count, _x, _y, _text, _event)	\
	(list_init((_list), 0, (_count), (_x), (_y), (_text), 0xff000000, 0xff999999, 0xff000000, 0xffbbbbbb, (_event)))

#define	LIST(_list, _image, _params, _top)	\
	(list((_list), (_image), (_params), (_top)))

#define	SET_LIST_TEXT(_list, _index, _text)	\
	(list_set_text((_list), (_index), (_text)))

#define	ENABLE_LIST(_list)	\
	(list_enable((_list)))

#define	DISABLE_LIST(_list)	\
	(list_disable((_list)))

extern
BOOL
list_init(	OUT ListPtr list,
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
list(	IN OUT ListPtr list,
		OUT ImagePtr image,
		IN WindowEventParamsPtr params,
		BOOL top);

extern
BOOL
list_set_text(	IN OUT ListPtr list,
				IN uint32 index,
				IN CASCTEXT text);

extern
BOOL
list_enable(IN OUT ListPtr list);

extern
BOOL
list_disable(IN OUT ListPtr list);

#endif
