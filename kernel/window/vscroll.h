/**
	@File:			vscroll.h
	@Author:		Ihsoh
	@Date:			2020-05-05
	@Description:
		窗体控件 - Vertical Scroll。
		垂直滚动条。
*/

#ifndef	_WINDOW_VSCROLL_H_
#define	_WINDOW_VSCROLL_H_

#include "control.h"

#include "../types.h"
#include "../image.h"
#include "../window.h"

#define	VSCROLL_BORDER_WIDTH		1
#define	VSCROLL_BORDER_WIDTH2X	(2 * VSCROLL_BORDER_WIDTH)

typedef struct
{
	uint32				id;
	int32				type;
	int32				uwid;							// User Window ID。
	int32				uwcid;							// User Window Control ID。
	uint32				value;
	uint32				min;
	uint32				max;
	uint32				x;
	uint32				y;
	uint32				width;
	uint32				height;
	uint32				color;
	uint32				bgcolor;
	ControlEvent		event;
	BOOL				updated;
	BOOL				enabled;

} VScroll, * VScrollPtr;

#define	INIT_VSCROLL(_scroll, _x, _y, _width, _height, _value, _min, _max, _event)	\
	(CtrlVScrlInit((_scroll), 0, (_value), (_min), (_max), (_x), (_y), (_width), (_height), 0xffaaaaaa, 0xff555555, (_event)))

#define	VSCROLL(_scroll, _image, _params, _top)	\
	(CtrlVScrlUpdate((_scroll), (_image), (_params), (_top)))

#define	SET_VSCROLL_VALUE(_scroll, _value)	\
	(CtrlVScrlSetValue((_scroll), (_value)))

#define	ENABLE_VSCROLL(_scroll)	\
	((_scroll)->enabled = TRUE)

#define	DISABLE_VSCROLL(_scroll)	\
	((_scroll)->enabled = FALSE)

#define	GET_VSCROLL_WIDTH(_scroll)	\
	((_scroll)->width)

#define	GET_VSCROLL_HEIGHT(_scroll)	\
	((_scroll)->height)

extern
BOOL
CtrlVScrlInit(	OUT VScrollPtr scroll,
				IN uint32 id,
				IN uint32 value,
				IN uint32 min,
				IN uint32 max,
				IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 color,
				IN uint32 bgcolor,
				IN ControlEvent event);

extern
BOOL
CtrlVScrlUpdate(IN OUT VScrollPtr scroll,
				OUT ImagePtr image,
				IN WindowEventParamsPtr params,
				IN BOOL top);

extern
BOOL
CtrlVScrlSetValue(	IN OUT VScrollPtr scroll,
					IN uint32 value);

#endif
