/**
	@File:			scroll.h
	@Author:		Ihsoh
	@Date:			2015-09-12
	@Description:
		窗体控件 - Scroll。
*/

#ifndef	_WINDOW_SCROLL_H_
#define	_WINDOW_SCROLL_H_

#include "control.h"

#include "../types.h"
#include "../image.h"
#include "../window.h"

typedef struct
{
	uint32				id;
	int32				type;
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

} Scroll, * ScrollPtr;

#define	SCROLL_CHANGED	1

#define	SCROLL(_scroll, _image, _params, _top)	\
	(scroll((_scroll), (_image), (_params), (_top)))

#endif
