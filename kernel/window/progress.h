/**
	@File:			progress.h
	@Author:		Ihsoh
	@Date:			2015-09-03
	@Description:
		窗体控件 - Progress。
*/

#ifndef	_WINDOW_PROGRESS_H_
#define	_WINDOW_PROGRESS_H_

#include "control.h"

#include "../types.h"
#include "../image.h"
#include "../window.h"

typedef struct
{
	uint32				id;
	int32				type;
	int32				uwid;							// User Window ID。
	int32				uwcid;							// User Window Control ID。
	uint32				percent;
	uint32				x;
	uint32				y;
	uint32				width;
	uint32				height;
	uint32				color;
	uint32				bgcolor;
	ControlEvent		event;
	BOOL				updated;

} Progress, * ProgressPtr;

#define	INIT_PROGRESS(_progress, _x, _y, _width, _height, _percent, _event)	\
	(CtrlPrgrInit((_progress), 0, (_percent), (_x), (_y), (_width), (_height), 0xffaaaaaa, 0xff555555, (_event)))

#define	PROGRESS(_progress, _image, _params, _top)	\
	(CtrlPrgrUpdate((_progress), (_image), (_params), (_top)))

#define	SET_PROGRESS_PERCENT(_progress, _percent)	\
	(CtrlPrgrSetPercent((_progress), (_percent)))

#define	GET_PROGRESS_WIDTH(_progress)	\
	((_progress)->width)

#define	GET_PROGRESS_HEIGHT(_progress)	\
	((_progress)->height)

extern
BOOL
CtrlPrgrInit(	OUT ProgressPtr progress,
				IN uint32 id,
				IN uint32 percent,
				IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 color,
				IN uint32 bgcolor,
				IN ControlEvent event);

extern
BOOL
CtrlPrgrUpdate(	IN OUT ProgressPtr progress,
				OUT ImagePtr image,
				IN WindowEventParamsPtr params,
				IN BOOL top);

extern
BOOL
CtrlPrgrSetPercent(	OUT ProgressPtr progress,
					IN uint32 percent);

#endif
