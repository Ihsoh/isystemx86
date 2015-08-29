/**
	@File:			label.h
	@Author:		Ihsoh
	@Date:			2015-08-30
	@Description:
		窗体控件 - Label。
*/

#ifndef	_WINDOW_LABEL_H_
#define	_WINDOW_LABEL_H_

#include "control.h"

#include "../types.h"
#include "../image.h"
#include "../window.h"

typedef	struct
{
	int32				id;
	uint32				x;
	uint32				y;
	CASCTEXT			text;
	uint32				color;
	uint32				bgcolor;
	uint32				colorh;
	uint32				bgcolorh;
	ControlEvent		event;
	uint32				width;
	uint32				height;
	BOOL				lbdown;
	BOOL				rbdown;
	BOOL				hover;
	uint32				max_row;
	uint32				max_col;
} Label, * LabelPtr;

#define	LABEL_LPADDING	6
#define	LABEL_RPADDING	6
#define	LABEL_TPADDING	4
#define	LABEL_BPADDING	4

#define	LABEL_LBDOWN	1
#define	LABEL_LBUP		2
#define	LABEL_CLICK		LABEL_LBUP
#define	LABEL_RBDOWN	3
#define	LABEL_RBUP		4
#define	LABEL_HOVER		5
#define	LABEL_IN 		6
#define	LABEL_OUT		7

extern
BOOL
label_init(	OUT LabelPtr label,
			IN int32 id,
			IN uint32 x,
			IN uint32 y,
			IN CASCTEXT text,
			IN uint32 color,
			IN uint32 bgcolor,
			IN uint32 colorh,
			IN uint32 bgcolorh,
			IN ControlEvent event,
			IN uint32 width,
			IN uint32 height);

extern
BOOL
label(	IN OUT LabelPtr label,
		OUT ImagePtr image,
		IN WindowEventParamsPtr params,
		BOOL top);

#endif
