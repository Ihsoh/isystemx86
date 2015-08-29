/**
	@File:			label.c
	@Author:		Ihsoh
	@Date:			2015-08-30
	@Description:
		窗体控件 - Label。
*/

#include "label.h"
#include "control.h"
#include "../types.h"
#include "../image.h"
#include "../enfont.h"
#include "../klib.h"
#include "../window.h"
#include "../kernel.h"

#include <ilib/string.h>

//static
uint32
_max(	IN CASCTEXT text,
		OUT uint32 * row,
		OUT uint32 * col)
{
	ASCCHAR chr = '\0';
	uint32 r = 0, c = 0, cc = 0;
	uint32 len = strlen(text);
	uint32 ui;
	for(ui = 0; ui < len; ui++)
	{
		chr = text[ui];
		if(chr == '\n')
		{
			if(cc > c)
				c = cc;
			cc = 0;
			r++;
		}
		else
			cc++;
	}
	if(chr != '\n' && chr != '\0')
	{
		if(cc > c)
			c = cc;
		r++;
	}
	*row = r;
	*col = c;
}

/**
	@Function:		label_init
	@Access:		Public
	@Description:
		初始化Label。
	@Parameters:
		label, LabelPtr, IN
			指向Label对象的指针。
		id, int32, IN
			Label的ID。
		x, uint32, IN
			Label的X坐标。
		y, uint32, IN
			Label的Y坐标。
		text, CASCTEXT, IN
			Label的文本。
		color, uint32, IN
			鼠标不在Label上方时，Label的文本的颜色。
		bgcolor, uint32, IN
			鼠标不在Label上方时，Label的背景颜色。
		colorh, uint32, IN
			鼠标在Label上方时，Label的文本的颜色。
		bgcolorh, uint32, IN
			鼠标在Label上方时，Label的背景颜色。
		event, ControlEvent, IN
			Label的事件函数。
		width, uint32, IN
			Label的宽度。如果为0则不起作用。
		height, uint32, IN
			Label的高度。如果为0则不起作用。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
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
			IN uint32 height)
{
	if(label == NULL)
		return FALSE;
	label->id = id;
	label->x = x;
	label->y = y;
	label->text = text;
	label->color = color;
	label->bgcolor = bgcolor;
	label->colorh = colorh;
	label->bgcolorh = bgcolorh;
	label->event = event;
	label->width = width;
	label->height = height;
	label->lbdown = FALSE;
	label->rbdown = FALSE;
	label->hover = FALSE;
	return TRUE;
}

/**
	@Function:		label
	@Access:		Public
	@Description:
		Label的渲染、事件处理函数。
	@Parameters:
		label, LabelPtr, IN OUT
			指向Label对象的指针。
		image, ImagePtr, OUT
			指向目标图片对象的指针。
		params, WindowEventParamsPtr, IN
			指向窗口事件参数的指针。
		top, BOOL, IN
			指示Label所在的窗体是否在最顶层。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
label(	IN OUT LabelPtr label,
		OUT ImagePtr image,
		IN WindowEventParamsPtr params,
		BOOL top)
{
	if(label == NULL || image == NULL || params == NULL)
		return FALSE;
	uint8 * enfont = get_enfont_ptr();
	uint32 id = label->id;
	uint32 x = label->x;
	uint32 y = label->y;
	CASCTEXT text = label->text;
	uint32 color = label->color;
	uint32 bgcolor = label->bgcolor;
	uint32 colorh = label->colorh;
	uint32 bgcolorh = label->bgcolorh;
	ControlEvent event = label->event;
	uint32 len = strlen(text);
	uint32 width = 0;
	if(label->width == 0)
		width = LABEL_LPADDING + len * ENFONT_WIDTH + LABEL_RPADDING;
	else
		width = label->width;
	uint32 height = 0;
	if(label->height == 0)
		height = LABEL_TPADDING + ENFONT_HEIGHT + LABEL_BPADDING;
	else
		height = label->height;
	if(point_in_rect(	params->mouse_x, params->mouse_y,
						x, y,
						width, height))
	{
		rect_common_image(	image,
							x, y,
							width, height,
							bgcolorh);
		text_common_image(	image,
							x + LABEL_LPADDING, y + LABEL_TPADDING,
							enfont, text, len, colorh);
		
		// LABEL_IN。
		if(!label->hover)
		{
			event(id, LABEL_IN, NULL);
			label->hover = TRUE;
		}

		// LABEL_HOVER。
		Point point;
		get_mouse_position(&point.x, &point.y);
		event(id, LABEL_HOVER, &point);

		if(is_mouse_left_button_down())
		{
			if(!label->lbdown && top)
				event(id, LABEL_LBDOWN, NULL);
			label->lbdown = TRUE;
		}
		else
			if(label->lbdown && top)
			{
				if(event != NULL)
					event(id, LABEL_LBUP, NULL);
				label->lbdown = FALSE;
			}
		if(is_mouse_right_button_down())
		{
			if(!label->rbdown && top)
				event(id, LABEL_RBDOWN, NULL);
			label->rbdown = TRUE;
		}
		else
			if(label->rbdown && top)
			{
				if(event != NULL)
					event(id, LABEL_RBUP, NULL);
				label->rbdown = FALSE;
			}
	}
	else
	{
		rect_common_image(	image, x, y, width, height, bgcolor);
		text_common_image(	image,
							x + LABEL_LPADDING, y + LABEL_TPADDING,
							enfont, text, len, color);

		// LABEL_OUT。
		if(label->hover)
		{
			event(id, LABEL_OUT, NULL);
			label->hover = FALSE;
		}

		label->lbdown = FALSE;
		label->rbdown = FALSE;
	}
	return TRUE;
}
