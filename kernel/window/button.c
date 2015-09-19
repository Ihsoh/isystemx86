/**
	@File:			button.c
	@Author:		Ihsoh
	@Date:			2015-08-30
	@Description:
		窗体控件 - Button。
*/

#include "button.h"
#include "control.h"
#include "../types.h"
#include "../image.h"
#include "../enfont.h"
#include "../klib.h"
#include "../window.h"
#include "../kernel.h"

#include <ilib/string.h>

/**
	@Function:		button_init
	@Access:		Public
	@Description:
		初始化Button。
	@Parameters:
		button, ButtonPtr, OUT
			指向Button对象的指针。
		id, uint32, IN
			Button的ID。
		x, uint32, IN
			Button的X坐标。
		y, uint32, IN
			Button的Y坐标。
		text, CASCTEXT, IN
			Button的文本。
		color, uint32, IN
			鼠标不在Button上方时，Button的文本的颜色。
		bgcolor, uint32, IN
			鼠标不在Button上方时，Button的背景颜色。
		colorh, uint32, IN
			鼠标在Button上方时，Button的文本的颜色。
		bgcolorh, uint32, IN
			鼠标在Button上方时，Button的背景颜色。
		event, ControlEvent, IN
			Button的事件函数。
		width, uint32, IN
			Button的宽度。如果为0则不起作用。
		height, uint32, IN
			Button的高度。如果为0则不起作用。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
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
			IN uint32 height)
{
	if(	button == NULL
		|| text == NULL
		|| strlen(text) > MAX_BUTTON_TEXT_LEN)
		return FALSE;
	if(id == 0)
		button->id = (uint32)button;
	else
		button->id = id;
	button->type = CONTROL_BUTTON;
	button->x = x;
	button->y = y;
	strcpy(button->text, text);
	button->color = color;
	button->bgcolor = bgcolor;
	button->colorh = colorh;
	button->bgcolorh = bgcolorh;
	button->event = event;
	button->width = width;
	button->height = height;
	button->lbdown = FALSE;
	button->rbdown = FALSE;
	button->hover = FALSE;
	button->len = strlen(text);
	button->clean = FALSE;
	button->old_len = 0;
	button->enabled = TRUE;
	button->vpext = NULL;
	return TRUE;
}

/**
	@Function:		button
	@Access:		Public
	@Description:
		Button的渲染、事件处理函数。
	@Parameters:
		button, ButtonPtr, IN OUT
			指向Button对象的指针。
		image, ImagePtr, OUT
			指向目标图片对象的指针。
		params, WindowEventParamsPtr, IN
			指向窗口事件参数的指针。
		top, BOOL, IN
			指示Button所在的窗体是否在最顶层。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
button(	IN OUT ButtonPtr button,
		OUT ImagePtr image,
		IN WindowEventParamsPtr params,
		BOOL top)
{
	if(button == NULL || image == NULL || params == NULL)
		return FALSE;
	if(!top && !(button->style & BUTTON_STYLE_REFRESH))
		return TRUE;
	uint8 * enfont = get_enfont_ptr();
	uint32 id = button->id;
	uint32 x = button->x;
	uint32 y = button->y;
	CASCTEXT text = button->text;
	uint32 color = button->color;
	uint32 bgcolor = button->bgcolor;
	uint32 colorh = button->colorh;
	uint32 bgcolorh = button->bgcolorh;
	ControlEvent event = NULL;
	if(button->enabled)
		event = button->event;
	else
		event = __dummy_event;
	uint32 len = button->len;
	uint32 width = 0;
	if(button->width == 0)
		width = BUTTON_LPADDING + len * ENFONT_WIDTH + BUTTON_RPADDING;
	else
		width = button->width;
	uint32 height = 0;
	if(button->height == 0)
		height = BUTTON_HEIGHT;
	else
		height = button->height;
	if(button->clean)
	{
		uint32 old_width = BUTTON_LPADDING + button->old_len * ENFONT_WIDTH + BUTTON_RPADDING;
		if(old_width > width)
			rect_common_image(	image,
								x, y,
								old_width, height,
								WINDOW_DEFBGCOLOR);
		button->clean = FALSE;
		button->old_len = 0;
	}
	if(	point_in_rect(	params->mouse_x, params->mouse_y,
						x, y,
						width, height)
		&& button->enabled)
	{
		rect_common_image(	image,
							x, y,
							width, height,
							bgcolorh);
		text_common_image(	image,
							x + BUTTON_LPADDING, y + BUTTON_TPADDING,
							enfont, text, len, colorh);
		
		// BUTTON_IN。
		if(!button->hover)
		{
			event(id, BUTTON_IN, NULL);
			button->hover = TRUE;
		}

		// BUTTON_HOVER。
		Point point;
		get_mouse_position(&point.x, &point.y);
		event(id, BUTTON_HOVER, &point);

		if(is_mouse_left_button_down())
		{
			if(!button->lbdown && top)
				event(id, BUTTON_LBDOWN, NULL);
			button->lbdown = TRUE;
		}
		else
			if(button->lbdown && top)
			{
				event(id, BUTTON_LBUP, NULL);
				button->lbdown = FALSE;
			}
		if(is_mouse_right_button_down())
		{
			if(!button->rbdown && top)
				event(id, BUTTON_RBDOWN, NULL);
			button->rbdown = TRUE;
		}
		else
			if(button->rbdown && top)
			{
				event(id, BUTTON_RBUP, NULL);
				button->rbdown = FALSE;
			}
	}
	else
	{
		rect_common_image(	image, x, y, width, height, bgcolor);
		text_common_image(	image,
							x + BUTTON_LPADDING, y + BUTTON_TPADDING,
							enfont, text, len, color);

		// BUTTON_OUT。
		if(button->hover)
		{
			event(id, BUTTON_OUT, NULL);
			button->hover = FALSE;
		}

		button->lbdown = FALSE;
		button->rbdown = FALSE;
	}
	return TRUE;
}

/**
	@Function:		button_set_text
	@Access:		Public
	@Description:
		设置Button的文本。
	@Parameters:
		button, ButtonPtr, IN OUT
			指向Button对象的指针。
		text, CASCTEXT, IN
			Button的文本。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
button_set_text(OUT ButtonPtr button,
				IN CASCTEXT text)
{
	if(	button == NULL
		|| text == NULL
		|| strlen(text) > MAX_BUTTON_TEXT_LEN)
		return FALSE;
	strcpy(button->text, text);
	button->old_len = button->len;
	button->len = strlen(text);
	button->clean = TRUE;
	return TRUE;
}
