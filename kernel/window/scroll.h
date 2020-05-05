/**
	@File:			scroll.h
	@Author:		Ihsoh
	@Date:			2015-09-12
	@Description:
		窗体控件 - Horizontal Scroll。
		水平滚动条。
*/

#ifndef	_WINDOW_SCROLL_H_
#define	_WINDOW_SCROLL_H_

#include "control.h"

#include "../types.h"
#include "../image.h"
#include "../window.h"

#define	SCROLL_BORDER_WIDTH		1
#define	SCROLL_BORDER_WIDTH2X	(2 * SCROLL_BORDER_WIDTH)

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

	void *				vpext;
} Scroll, * ScrollPtr;

#define	INIT_SCROLL(_scroll, _x, _y, _width, _height, _value, _min, _max, _event)	\
	(CtrlScrlInit((_scroll), 0, (_value), (_min), (_max), (_x), (_y), (_width), (_height), 0xffaaaaaa, 0xff555555, (_event)))

#define	SCROLL(_scroll, _image, _params, _top)	\
	(CtrlScrlUpdate((_scroll), (_image), (_params), (_top)))

#define	SET_SCROLL_VALUE(_scroll, _value)	\
	(CtrlScrlSetValue((_scroll), (_value)))

#define	ENABLE_SCROLL(_scroll)	\
	((_scroll)->enabled = TRUE)

#define	DISABLE_SCROLL(_scroll)	\
	((_scroll)->enabled = FALSE)

#define	GET_SCROLL_WIDTH(_scroll)	\
	((_scroll)->width)

#define	GET_SCROLL_HEIGHT(_scroll)	\
	((_scroll)->height)

extern
BOOL
CtrlScrlInit(	OUT ScrollPtr scroll,
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
CtrlScrlUpdate(	IN OUT ScrollPtr scroll,
				OUT ImagePtr image,
				IN WindowEventParamsPtr params,
				IN BOOL top);

extern
BOOL
CtrlScrlSetValue(	IN OUT ScrollPtr scroll,
					IN uint32 value);

#endif
