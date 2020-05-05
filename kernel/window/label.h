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

#define	MAX_LABEL_TEXT_LEN	65536

typedef	struct
{
	uint32				id;
	int32				type;
	int32				uwid;							// User Window ID。
	int32				uwcid;							// User Window Control ID。
	uint32				x;
	uint32				y;
	ASCCHAR				text[MAX_LABEL_TEXT_LEN + 1];
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
	BOOL				clean;
	uint32				old_max_row;
	uint32				old_max_col;
	BOOL				enabled;
	uint32				style;
	void *				vpext;
} Label, * LabelPtr;

#define	LABEL_LPADDING	6
#define	LABEL_RPADDING	6
#define	LABEL_TPADDING	4
#define	LABEL_BPADDING	4

#define	LABEL_STYLE_REFRESH		0x00000001

#define	INIT_LABEL(_label, _x, _y, _text, _event)	\
	(CtrlLblInit((_label), 0, (_x), (_y), (_text),	\
		0xff000000, WINDOW_DEFBGCOLOR, 0xff000000, WINDOW_DEFBGCOLOR, (_event), 0, 0))

#define	LABEL(_label, _image, _params, _top)	\
	(CtrlLblUpdate((_label), (_image), (_params), (_top)))

#define	SET_LABEL_TEXT(_label, _text)	\
	(CtrlLblSetText((_label), (_text)))

#define	ENABLE_LABEL(_label)	\
	((_label)->enabled = TRUE)

#define	DISABLE_LABEL(_label)	\
	((_label)->enabled = FALSE)

#define	GET_LABEL_WIDTH(_label)	\
	((_label)->width)

#define	GET_LABEL_HEIGHT(_label)	\
	((_label)->height)

extern
BOOL
CtrlLblInit(OUT LabelPtr label,
			IN uint32 id,
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
CtrlLblUpdate(	IN OUT LabelPtr label,
				OUT ImagePtr image,
				IN WindowEventParamsPtr params,
				BOOL top);

extern
BOOL
CtrlLblSetText(	OUT LabelPtr label,
				IN CASCTEXT text);

#endif
