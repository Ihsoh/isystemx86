/**
	@File:			button.h
	@Author:		Ihsoh
	@Date:			2015-08-30
	@Description:
		窗体控件 - 按钮。
*/

#ifndef	_WINDOW_BUTTON_H_
#define	_WINDOW_BUTTON_H_

#include "../types.h"
#include "../image.h"
#include "../window.h"

typedef void (* ButtonEvent)(int32 id, uint32 type, void * param);

typedef struct
{
	int32				id;
	uint32				x;
	uint32				y;
	CASCTEXT			text;
	uint32				color;
	uint32				bgcolor;
	uint32				colorh;
	uint32				bgcolorh;
	ButtonEvent			event;
	uint32				width;
	uint32				height;
	BOOL				lbdown;
	BOOL				rbdown;
	BOOL				hover;
} Button, * ButtonPtr;

#define	BUTTON_LPADDING	6
#define	BUTTON_RPADDING	6
#define	BUTTON_TPADDING	4
#define	BUTTON_BPADDING	4

#define	BUTTON_LBDOWN		1
#define	BUTTON_LBUP			2
#define	BUTTON_CLICK		BUTTON_LBUP
#define	BUTTON_RBDOWN		3
#define	BUTTON_RBUP			4
#define	BUTTON_HOVER		5
#define	BUTTON_IN 			6
#define	BUTTON_OUT			7

#define	INIT_BUTTON(_button, _id, _x, _y, _text, _event)	\
	(button_init(_button, _id, _x, _y, _text, 0xff000000, 0xff999999, 0xff000000, 0xffbbbbbb, _event, 0, 0))

#define	BUTTON(_button, _image, _params, _top)	\
	(button(_button, _image, _params, _top))

extern
BOOL
button_init(OUT ButtonPtr button,
			IN int32 id,
			IN uint32 x,
			IN uint32 y,
			IN CASCTEXT text,
			IN uint32 color,
			IN uint32 bgcolor,
			IN uint32 colorh,
			IN uint32 bgcolorh,
			IN ButtonEvent event,
			IN uint32 width,
			IN uint32 height);

extern
BOOL
button(	IN OUT ButtonPtr button,
		OUT ImagePtr image,
		IN WindowEventParamsPtr params,
		BOOL top);

#endif
