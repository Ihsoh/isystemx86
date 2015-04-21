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

/**
	@Function:		get_days_of_month
	@Access:		Public
	@Description:
		获取某年某月有多少天。
	@Parameters:
		year, uint16, IN
			年。
		month, uint8, IN
			月。
	@Return:
		uint32:
			返回0xff则表示发生错误，
			否则返回某年某月的天数。		
*/
uint8
get_days_of_month(	IN uint16 year,
					IN uint8 month)
{
	uint8 days;
	switch(month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			days = 31;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			days = 30;
			break;
		case 2:
			days = 28;
			break;
		default:
			return 0xff;
	}
	if(	month == 2 
		&& (year % 4 == 0 && year % 100 != 0) 
		|| year % 400 == 0)
		days++;
	return days;
}

/**
	@Function:		get_day_of_week
	@Access:		Public
	@Description:
		获取某年某月某日是星期几。
	@Parameters:
		year, uint16, IN
			年。
		month, uint8, IN
			月。
		day, uint8, IN
			日。
	@Return:
		BOOL
			返回某年某月某日的星期(0~6, Sunday~Saturday)。		
*/
uint8
get_day_of_week(IN uint16 year,
				IN uint8 month,
				IN uint8 day)
{
	int32 y = year;
	int32 m = month;
	int32 d = day;
	if(m == 1 || m == 2)
	{
		y--;
		m += 12;
	}
	int32 t0 = y / 100;
	int32 t1 = y - t0 * 100;
	int32 w = (t0 / 4) - 2 * t0 + (t1 + t1 / 4) + (13 * (m + 1) / 5) + d - 1;
	while(w < 0)
		w += 7;
	w %= 7;
	return w;
}
