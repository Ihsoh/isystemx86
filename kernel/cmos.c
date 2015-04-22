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
#include "cmlock.h"

static BOOL rtc_state = FALSE;	//如果RTC被启用则为TRUE，否则为FALSE。

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
BOOL 
get_cmos_date_time(OUT struct CMOSDateTime * dt)
{
	if(dt == NULL)
		return FALSE;
	common_lock();
	uint8 temp;
	//秒。
	outb(0x70, 0x00);
	temp = inb(0x71);
	dt->second = (temp >> 4) * 10 + (temp & 0x0f);
	//分。
	outb(0x70, 0x02);
	temp = inb(0x71);
	dt->minute = (temp >> 4) * 10 + (temp & 0x0f);
	//时。
	outb(0x70, 0x04);
	temp = inb(0x71);
	dt->hour = (temp >> 4) * 10 + (temp & 0x0f);
	//星期。
	outb(0x70, 0x06);
	temp = inb(0x71);
	dt->day_of_week = (temp >> 4) * 10 + (temp & 0x0f);
	//日。
	outb(0x70, 0x07);
	temp = inb(0x71);
	dt->day = (temp >> 4) * 10 + (temp & 0x0f);
	//月。
	outb(0x70, 0x08);
	temp = inb(0x71);
	dt->month = (temp >> 4) * 10 + (temp & 0x0f);
	//年。
	outb(0x70, 0x09);
	temp = inb(0x71);
	dt->year = (temp >> 4) * 10 + (temp & 0x0f);
	//世纪。
	outb(0x70, 0x32);
	temp = inb(0x71);
	dt->year += ((temp >> 4) * 10 + (temp & 0x0f)) * 100;
	common_unlock();
	return TRUE;
}

/**
	@Function:		to_bcd8
	@Access:		Public
	@Description:
		把一个两位的十进制数转换为BCD数。
	@Parameters:
		n, uint8, IN
			两位的十进制数。
	@Return:
		uint8
			BCD数。
*/
static
uint8
to_bcd8(uint8 n)
{
	uint8 n0 = n % 10;
	uint8 n1 = (uint8)(n / 10) % 10;
	return (n1 << 4) | n0;
}

/**
	@Function:		set_cmos_date_time
	@Access:		Public
	@Description:
		设置CMOS时间。
	@Parameters:
		dt, struct CMOSDateTime *, OUT
			指向用于储存时间的缓冲区。
	@Return:	
*/
BOOL
set_cmos_date_time(IN struct CMOSDateTime * dt)
{
	if(dt == NULL)
		return FALSE;
	uint8 days = get_days_of_month(dt->year, dt->month);
	if(	days == 0xff 
		|| dt->day == 0
		|| dt->day > days
		|| dt->hour >= 60
		|| dt->minute >= 60
		|| dt->second >= 60)
		return FALSE;
	dt->day_of_week = get_day_of_week(dt->year, dt->month, dt->day);
	common_lock();
	uint8 temp;
	//秒。
	outb(0x70, 0x00);
	outb(0x71, to_bcd8(dt->second));
	//分。
	outb(0x70, 0x02);
	outb(0x71, to_bcd8(dt->minute));
	//时。
	outb(0x70, 0x04);
	outb(0x71, to_bcd8(dt->hour));
	//星期。
	outb(0x70, 0x06);
	outb(0x71, to_bcd8(dt->day_of_week));
	//日。
	outb(0x70, 0x07);
	outb(0x71, to_bcd8(dt->day));
	//月。
	outb(0x70, 0x08);
	outb(0x71, to_bcd8(dt->month));
	//年。
	outb(0x70, 0x09);
	outb(0x71, to_bcd8(dt->year % 100));
	//世纪。
	outb(0x70, 0x32);
	outb(0x71, to_bcd8(dt->year / 100));
	common_unlock();
	return TRUE;
}

/**
	@Function:		set_cmos_date
	@Access:		Public
	@Description:
		设置CMOS的日期。
	@Parameters:
		year, uint16, IN
			年。
		month, uint8, IN
			月。
		day, uint8, IN
			日。
	@Return:	
*/
BOOL
set_cmos_date(	IN uint16 year,
				IN uint8 month,
				IN uint8 day)
{
	uint8 days = get_days_of_month(year, month);
	if(	days == 0xff 
		|| day == 0
		|| day > days)
		return FALSE;
	uint8 day_of_week = get_day_of_week(year, month, day);
	common_lock();
	//日。
	outb(0x70, 0x07);
	outb(0x71, to_bcd8(day));
	//月。
	outb(0x70, 0x08);
	outb(0x71, to_bcd8(month));
	//年。
	outb(0x70, 0x09);
	outb(0x71, to_bcd8(year % 100));
	//世纪。
	outb(0x70, 0x32);
	outb(0x71, to_bcd8(year / 100));
	common_unlock();
	return TRUE;
}

/**
	@Function:		set_cmos_time
	@Access:		Public
	@Description:
		设置CMOS的时间。
	@Parameters:
		hour, uint8, IN
			时。
		minute, uint8, IN
			分。
		second, uint8, IN
			秒。
	@Return:	
*/
BOOL
set_cmos_time(	IN uint8 hour,
				IN uint8 minute,
				IN uint8 second)
{
	if(	hour >= 60
		|| minute >= 60
		|| second >= 60)
		return FALSE;
	common_lock();
	//秒。
	outb(0x70, 0x00);
	outb(0x71, to_bcd8(second));
	//分。
	outb(0x70, 0x02);
	outb(0x71, to_bcd8(minute));
	//时。
	outb(0x70, 0x04);
	outb(0x71, to_bcd8(hour));
	common_unlock();
	return TRUE;
}

/**
	@Function:		get_rtc_state
	@Access:		Public
	@Description:
		获取RTC的状态。
	@Parameters:
	@Return:
		返回TRUE则开启，否则关闭中。
*/
BOOL
get_rtc_state(void)
{
	return rtc_state;
}

/**
	@Function:		enable_rtc
	@Access:		Public
	@Description:
		启用RTC。
	@Parameters:
	@Return:
		返回TRUE则开启成功，否则开启失败。
*/
BOOL
enable_rtc(void)
{
	if(!rtc_state)
	{
		common_lock();
		outb(0x70, 0x0b);
		uint8 prev = inb(0x71);
		outb(0x70, 0x0b);
		outb(0x71, prev | 0x40);
		rtc_state = TRUE;
		common_unlock();
	}
	return TRUE;
}

/**
	@Function:		disable_rtc
	@Access:		Public
	@Description:
		禁用RTC。
	@Parameters:
	@Return:
		返回TRUE则禁用成功，否则禁用失败。
*/
BOOL
disable_rtc(void)
{
	if(rtc_state)
	{
		common_lock();
		outb(0x70, 0x0b);
		uint8 prev = inb(0x71);
		outb(0x70, 0x0b);
		outb(0x71, prev & ~0x40);
		rtc_state = FALSE;
		common_unlock();
	}
	return TRUE;
}

/**
	@Function:		set_rtc_rate
	@Access:		Public
	@Description:
		设置RTC的频率。
	@Parameters:
	@Return:
		返回TRUE则设置成功，否则设置失败。
*/
BOOL
set_rtc_rate(uint8 rate)
{
	if(rate <= 2 || rate > 15)
		return FALSE;
	common_lock();
	outb(0x70, 0x0a);
	uint8 prev = inb(0x71);
	outb(0x70, 0x0a);
	outb(0x71, (prev & 0xf0) | rate);
	common_unlock();
	return TRUE;
}

/**
	@Function:		end_of_rtc
	@Access:		Public
	@Description:
		IRQ8处理程序结束时需要调用该函数，否则IRQ8不会再次引发。
	@Parameters:
	@Return:
*/
void
end_of_rtc(void)
{
	if(rtc_state)
	{
		common_lock();
		outb(0x70, 0x0c);
		inb(0x71);
		common_unlock();
	}
}

/**
	@Function:		enable_nmi
	@Access:		Public
	@Description:
		启用NMI。
	@Parameters:
	@Return:
*/
void
enable_nmi(void)
{
	outb(0x70, inb(0x70) & 0x7f);
}

/**
	@Function:		disable_nmi
	@Access:		Public
	@Description:
		禁用NMI。
	@Parameters:
	@Return:
*/
void
disable_nmi(void)
{
	outb(0x70, inb(0x70) | 0x80);
}
