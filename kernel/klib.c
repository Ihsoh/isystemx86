/**
	@File:			klib.c
	@Author:		Ihsoh
	@Date:			2014-12-12
	@Description:
		内核基本库。
*/

#include "types.h"
#include "klib.h"

/**
	@Function:		point_in_rect
	@Access:		Public
	@Description:
		检测点是否在矩形内。
	@Parameters:
		point_x, int32, IN
			点的 X 坐标。
		point_y, int32, IN
			点的 Y 坐标。
		rect_x, int32, IN
			矩形的 X 坐标。
		rect_y, int32, IN
			矩形的 Y 坐标。
		rect_w, uint32, IN
			矩形的宽度。
		rect_h, uint32, IN
			矩形的高度。
	@Return:
		BOOL
			返回TRUE则在矩形内，否则不在矩形内。		
*/
BOOL
point_in_rect(	IN int32 point_x,
				IN int32 point_y, 
				IN int32 rect_x,
				IN int32 rect_y,
				IN uint32 rect_w,
				IN uint32 rect_h)
{
	if(	point_x < rect_x
		|| point_y < rect_y
		|| point_x >= rect_x + rect_w
		|| point_y >= rect_y + rect_h)
		return FALSE;
	else
		return TRUE;
}
