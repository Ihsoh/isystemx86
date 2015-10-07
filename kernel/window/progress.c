/**
	@File:			progress.c
	@Author:		Ihsoh
	@Date:			2015-09-03
	@Description:
		窗体控件 - Progress。
*/

#include "progress.h"
#include "control.h"
#include "../types.h"
#include "../image.h"
#include "../enfont.h"
#include "../klib.h"
#include "../window.h"
#include "../kernel.h"

#include <ilib/string.h>

/**
	@Function:		progress_init
	@Access:		Public
	@Description:
		初始化Progress。
	@Parameters:
		progress, ProgressPtr, OUT
			指向Progress的指针。
		id, uint32, IN
			Progress的ID。
		percent, uint32, IN
			百分比值，范围为0 ~ 100。
		x, uint32, IN
			Progress的X坐标。
		y, uint32, IN
			Progress的Y坐标。
		width, uint32, IN
			Progress的宽度。
		height, uint32, IN
			Progress的高度。
		color, uint32, IN
			Progress的前景色。
		bgcolor, uint32, IN
			Progress的背景色。
		event, ControlEvent, IN
			Progress的事件函数。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
progress_init(	OUT ProgressPtr progress,
				IN uint32 id,
				IN uint32 percent,
				IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 color,
				IN uint32 bgcolor,
				IN ControlEvent event)
{
	if(progress == NULL)
		return FALSE;
	if(id == 0)
		progress->id = (uint32)progress;
	else
		progress->id = id;
	progress->type = CONTROL_PROGRESS;
	progress->uwid = -1;
	progress->uwcid = -1;
	progress->percent = percent;
	progress->x = x;
	progress->y = y;
	if(width < 2)
		progress->width = 2;
	else
		progress->width = width;
	if(height < 2)
		progress->height = 2;
	else
		progress->height = height;
	progress->color = color;
	progress->bgcolor = bgcolor;
	progress->event = event;
	progress->updated = FALSE;
	return TRUE;
}

/**
	@Function:		progress
	@Access:		Public
	@Description:
		Progress的渲染、事件处理函数。
	@Parameters:
		progress, ProgressPtr, IN OUT
			指向Progress对象的指针。
		image, ImagePtr, OUT
			指向目标图片对象的指针。
		params, WindowEventParamsPtr, IN
			指向窗口事件参数的指针。
		top, BOOL, IN
			指示Progress所在的窗体是否在最顶层。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
progress(	IN OUT ProgressPtr progress,
			OUT ImagePtr image,
			IN WindowEventParamsPtr params,
			BOOL top)
{
	if(progress == NULL || image == NULL || params == NULL)
		return FALSE;
	if(!top)
		return TRUE;
	if(!progress->updated)
	{
		hline_common_image(	image,
							progress->x, progress->y,
							progress->width,
							WINDOW_BORDERCOLOR);
		hline_common_image(	image,
							progress->x, progress->y + progress->height - 1,
							progress->width,
							WINDOW_BORDERCOLOR);
		vline_common_image(	image,
							progress->x, progress->y + 1,
							progress->height - 2,
							WINDOW_BORDERCOLOR);
		vline_common_image(	image,
							progress->x + progress->width - 1, progress->y + 1,
							progress->height - 2,
							WINDOW_BORDERCOLOR);
		rect_common_image(	image,
							progress->x + 1, progress->y + 1,
							progress->width - 2, progress->height - 2,
							progress->bgcolor);
		double w = ((double)progress->percent / 100.0) * (double)(progress->width - 2);
		rect_common_image(	image,
							progress->x + 1, progress->y + 1,
							(uint32)w, progress->height - 2,
							progress->color);
		progress->updated = TRUE;
	}
}

/**
	@Function:		progress_set_percent
	@Access:		Public
	@Description:
		设置Progress的进度的百分比。
		范围为0 ~ 100.
	@Parameters:
		progress, ProgressPtr, OUT
			指向Progress对象的指针。
		percent, uint32, IN
			Progress的进度的百分比。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
progress_set_percent(	OUT ProgressPtr progress,
						IN uint32 percent)
{
	if(progress == NULL || percent > 100)
		return FALSE;
	progress->percent = percent;
	progress->updated = FALSE;
	return TRUE;
}
