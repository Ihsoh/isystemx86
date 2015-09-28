/**
	@File:			scroll.c
	@Author:		Ihsoh
	@Date:			2015-09-12
	@Description:
		窗体控件 - Scroll。
*/

#include "scroll.h"
#include "control.h"
#include "../types.h"
#include "../image.h"
#include "../enfont.h"
#include "../klib.h"
#include "../window.h"
#include "../kernel.h"
#include "../mouse.h"

#include <ilib/string.h>

/**
	@Function:		scroll_init
	@Access:		Public
	@Description:
		初始化Scroll。
	@Parameters:
		scroll, ScrollPtr, OUT
			指向Scroll的指针。
		id, uint32, IN
			控件的ID。
		value, uint32, IN
			控件值，应该min <= value <= max。
		min, uint32, IN
			最小值。
		max, uint32, IN
			最大值。
		x, uint32, IN
			控件的X坐标。
		y, uint32, IN
			控件的Y坐标。
		width, uint32, IN
			控件的宽度。
		height, uint32, IN
			控件的高度。
		color, uint32, IN
			控件的前景色。
		bgcolor, uint32, IN
			控件的背景色。
		event, ControlEvent, IN
			控件的事件函数。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
scroll_init(OUT ScrollPtr scroll,
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
			IN ControlEvent event)
{
	if(	scroll == NULL
		|| min >= max
		|| value < min
		|| value > max)
		return FALSE;
	if(id == 0)
		scroll->id = (uint32)scroll;
	else
		scroll->id = id;
	scroll->type = CONTROL_SCROLL;
	scroll->value = value;
	scroll->min = min;
	scroll->max = max;
	scroll->x = x;
	scroll->y = y;
	if(width < 2)
		scroll->width = 2;
	else
		scroll->width = width;
	if(height < 2)
		scroll->height = 2;
	else
		scroll->height = height;
	scroll->color = color;
	scroll->bgcolor = bgcolor;
	scroll->event = event;
	scroll->updated = FALSE;
	return TRUE;
}

/**
	@Function:		scroll
	@Access:		Public
	@Description:
		Scroll的渲染、事件处理函数。
	@Parameters:
		scroll, ScrollPtr, IN OUT
			指向Scroll对象的指针。
		image, ImagePtr, OUT
			指向目标图片对象的指针。
		params, WindowEventParamsPtr, IN
			指向窗口事件参数的指针。
		top, BOOL, IN
			指示Scroll所在的窗体是否在最顶层。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
scroll(	IN OUT ScrollPtr scroll,
		OUT ImagePtr image,
		IN WindowEventParamsPtr params,
		BOOL top)
{
	if(scroll == NULL || image == NULL || params == NULL)
		return FALSE;
	if(!top)
		return TRUE;
	if(	point_in_rect(	params->mouse_x, params->mouse_y,
						scroll->x + 1, scroll->y + 1,
						scroll->width - 2, scroll->height - 2)
		&& (params->mouse_button & MOUSE_BUTTON_LEFT))
	{
		uint32 p = params->mouse_x - scroll->x;
		double sp = (double)p / (double)(scroll->width - 2);
		scroll->value = scroll->min + (uint32)(sp * (scroll->max - scroll->min));
		if(scroll->event != NULL)
			scroll->event(scroll->id, SCROLL_CHANGED, NULL);
		scroll->updated = FALSE;
	}
	if(!scroll->updated)
	{
		rect_common_image(	image,
							scroll->x + 1, scroll->y + 1,
							scroll->width - 2, scroll->height - 2,
							scroll->bgcolor);
		double sp = (double)scroll->value / (double)(scroll->max - scroll->min);	// 方块在滚动条上的比值。
		double sw = 1.0 / (double)(scroll->max - scroll->min);						// 方块的宽度的比值。
		double p = sp * (double)(scroll->width - 2);
		double w = sw * (double)(scroll->width - 2);
		// 画方块。
		rect_common_image(	image,
							scroll->x + 1 + (uint32)p, scroll->y + 1,
							(uint32)w, scroll->height - 2,
							scroll->color);

		// 画四条边。
		hline_common_image(	image,
							scroll->x, scroll->y,
							scroll->width,
							WINDOW_BORDERCOLOR);
		hline_common_image(	image,
							scroll->x, scroll->y + scroll->height - 1,
							scroll->width,
							WINDOW_BORDERCOLOR);
		vline_common_image(	image,
							scroll->x, scroll->y + 1,
							scroll->height - 2,
							WINDOW_BORDERCOLOR);
		vline_common_image(	image,
							scroll->x + scroll->width - 1, scroll->y + 1,
							scroll->height - 2,
							WINDOW_BORDERCOLOR);

		scroll->updated = TRUE;
	}
}
