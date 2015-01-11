/**
	@File:			cmos.c
	@Author:		Ihsoh
	@Date:			2014-7-17
	@Description:
		提供CMOS的相关功能。
*/

#include "cmos.h"
#include "types.h"
#include "386.h"

/**
	@Function:		get_cmos_date_time
	@Access:		Public
	@Description:
		获取CMOS时间。
	@Parameters:
		dt, struct CMOSDateTime *, OUT
			指向用于储存时间的缓冲区。
	@Return:	
*/
void 
get_cmos_date_time(OUT struct CMOSDateTime * dt)
{
	uint8 temp;
	//秒
	outb(0x70, 0x00);
	temp = inb(0x71);
	dt->second = (temp >> 4) * 10 + (temp & 0x0f);
	//分
	outb(0x70, 0x02);
	temp = inb(0x71);
	dt->minute = (temp >> 4) * 10 + (temp & 0x0f);
	//时
	outb(0x70, 0x04);
	temp = inb(0x71);
	dt->hour = (temp >> 4) * 10 + (temp & 0x0f);
	//星期
	outb(0x70, 0x06);
	temp = inb(0x71);
	dt->day_of_week = (temp >> 4) * 10 + (temp & 0x0f);
	//日
	outb(0x70, 0x07);
	temp = inb(0x71);
	dt->day = (temp >> 4) * 10 + (temp & 0x0f);
	//月
	outb(0x70, 0x08);
	temp = inb(0x71);
	dt->month = (temp >> 4) * 10 + (temp & 0x0f);
	//年
	outb(0x70, 0x09);
	temp = inb(0x71);
	dt->year = (temp >> 4) * 10 + (temp & 0x0f);
	outb(0x70, 0x32);
	temp = inb(0x71);
	dt->year += ((temp >> 4) * 10 + (temp & 0x0f)) * 100;
}
