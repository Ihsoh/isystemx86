/**
	@File:			button.h
	@Author:		Ihsoh
	@Date:			2015-08-30
	@Description:
		窗体控件 - Button。
*/

#ifndef	_WINDOW_BUTTON_H_
#define	_WINDOW_BUTTON_H_

#include "control.h"

#include "../types.h"
#include "../image.h"
#include "../window.h"

#define	MAX_BUTTON_TEXT_LEN		1023

typedef struct
{
	int32				id;
	int32				type;
	int32				uwid;	// User Window ID
	int32				uwcid;	// User Window Control ID
	uint32				x;
	uint32				y;
	ASCCHAR				text[MAX_BUTTON_TEXT_LEN + 1];
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
	uint32				len;
	BOOL				clean;
	uint32				old_len;
	BOOL				enabled;
	void *				vpext;
	uint32				style;
} Button, * ButtonPtr;

#define	BUTTON_LPADDING	6
#define	BUTTON_RPADDING	6
#define	BUTTON_TPADDING	4
#define	BUTTON_BPADDING	4

#define	BUTTON_HEIGHT	(BUTTON_TPADDING + ENFONT_HEIGHT + BUTTON_BPADDING)

#define	BUTTON_STYLE_REFRESH	0x00000001

#define	INIT_BUTTON(_button, _x, _y, _text, _event)	\
	(button_init((_button), 0, (_x), (_y), (_text),		\
		0xff000000, 0xff999999, 0xff000000, 0xffbbbbbb, (_event), 0, 0))

#define	BUTTON(_button, _image, _params, _top)	\
	(button((_button), (_image), (_params), (_top)))

#define	SET_BUTTON_TEXT(_button, _text)	\
	(button_set_text((_button), (_text)))

#define	ENABLE_BUTTON(_button)	\
	((_button)->enabled = TRUE)

#define	DISABLE_BUTTON(_button)	\
	((_button)->enabled = FALSE)

extern
BOOL
button_init(OUT ButtonPtr button,
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
button(	IN OUT ButtonPtr button,
		OUT ImagePtr image,
		IN WindowEventParamsPtr params,
		BOOL top);

extern
BOOL
button_set_text(OUT ButtonPtr button,
				IN CASCTEXT text);

#endif
