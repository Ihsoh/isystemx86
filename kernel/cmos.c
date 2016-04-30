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
	@Function:		CmosGetDateTime
	@Access:		Public
	@Description:
		获取CMOS时间。
	@Parameters:
		dt, struct CMOSDateTime *, OUT
			指向用于储存时间的缓冲区。
	@Return:	
*/
BOOL 
CmosGetDateTime(OUT struct CMOSDateTime * dt)
{
	if(dt == NULL)
		return FALSE;
	COMMON_LOCK();
	uint8 temp;
	//秒。
	KnlOutByte(0x70, 0x00);
	temp = KnlInByte(0x71);
	dt->second = (temp >> 4) * 10 + (temp & 0x0f);
	//分。
	KnlOutByte(0x70, 0x02);
	temp = KnlInByte(0x71);
	dt->minute = (temp >> 4) * 10 + (temp & 0x0f);
	//时。
	KnlOutByte(0x70, 0x04);
	temp = KnlInByte(0x71);
	dt->hour = (temp >> 4) * 10 + (temp & 0x0f);
	//星期。
	KnlOutByte(0x70, 0x06);
	temp = KnlInByte(0x71);
	dt->day_of_week = (temp >> 4) * 10 + (temp & 0x0f);
	//日。
	KnlOutByte(0x70, 0x07);
	temp = KnlInByte(0x71);
	dt->day = (temp >> 4) * 10 + (temp & 0x0f);
	//月。
	KnlOutByte(0x70, 0x08);
	temp = KnlInByte(0x71);
	dt->month = (temp >> 4) * 10 + (temp & 0x0f);
	//年。
	KnlOutByte(0x70, 0x09);
	temp = KnlInByte(0x71);
	dt->year = (temp >> 4) * 10 + (temp & 0x0f);
	//世纪。
	KnlOutByte(0x70, 0x32);
	temp = KnlInByte(0x71);
	dt->year += ((temp >> 4) * 10 + (temp & 0x0f)) * 100;
	COMMON_UNLOCK();
	return TRUE;
}

/**
	@Function:		_CmosToBcd8
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
_CmosToBcd8(uint8 n)
{
	uint8 n0 = n % 10;
	uint8 n1 = (uint8)(n / 10) % 10;
	return (n1 << 4) | n0;
}

/**
	@Function:		CmosSetDateTime
	@Access:		Public
	@Description:
		设置CMOS时间。
	@Parameters:
		dt, struct CMOSDateTime *, OUT
			指向用于储存时间的缓冲区。
	@Return:	
*/
BOOL
CmosSetDateTime(IN struct CMOSDateTime * dt)
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
	COMMON_LOCK();
	uint8 temp;
	//秒。
	KnlOutByte(0x70, 0x00);
	KnlOutByte(0x71, _CmosToBcd8(dt->second));
	//分。
	KnlOutByte(0x70, 0x02);
	KnlOutByte(0x71, _CmosToBcd8(dt->minute));
	//时。
	KnlOutByte(0x70, 0x04);
	KnlOutByte(0x71, _CmosToBcd8(dt->hour));
	//星期。
	KnlOutByte(0x70, 0x06);
	KnlOutByte(0x71, _CmosToBcd8(dt->day_of_week));
	//日。
	KnlOutByte(0x70, 0x07);
	KnlOutByte(0x71, _CmosToBcd8(dt->day));
	//月。
	KnlOutByte(0x70, 0x08);
	KnlOutByte(0x71, _CmosToBcd8(dt->month));
	//年。
	KnlOutByte(0x70, 0x09);
	KnlOutByte(0x71, _CmosToBcd8(dt->year % 100));
	//世纪。
	KnlOutByte(0x70, 0x32);
	KnlOutByte(0x71, _CmosToBcd8(dt->year / 100));
	COMMON_UNLOCK();
	return TRUE;
}

/**
	@Function:		CmosSetDate
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
CmosSetDate(IN uint16 year,
			IN uint8 month,
			IN uint8 day)
{
	uint8 days = get_days_of_month(year, month);
	if(	days == 0xff 
		|| day == 0
		|| day > days)
		return FALSE;
	uint8 day_of_week = get_day_of_week(year, month, day);
	COMMON_LOCK();
	//日。
	KnlOutByte(0x70, 0x07);
	KnlOutByte(0x71, _CmosToBcd8(day));
	//月。
	KnlOutByte(0x70, 0x08);
	KnlOutByte(0x71, _CmosToBcd8(month));
	//年。
	KnlOutByte(0x70, 0x09);
	KnlOutByte(0x71, _CmosToBcd8(year % 100));
	//世纪。
	KnlOutByte(0x70, 0x32);
	KnlOutByte(0x71, _CmosToBcd8(year / 100));
	COMMON_UNLOCK();
	return TRUE;
}

/**
	@Function:		CmosSetTime
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
CmosSetTime(IN uint8 hour,
			IN uint8 minute,
			IN uint8 second)
{
	if(	hour >= 60
		|| minute >= 60
		|| second >= 60)
		return FALSE;
	COMMON_LOCK();
	//秒。
	KnlOutByte(0x70, 0x00);
	KnlOutByte(0x71, _CmosToBcd8(second));
	//分。
	KnlOutByte(0x70, 0x02);
	KnlOutByte(0x71, _CmosToBcd8(minute));
	//时。
	KnlOutByte(0x70, 0x04);
	KnlOutByte(0x71, _CmosToBcd8(hour));
	COMMON_UNLOCK();
	return TRUE;
}

/**
	@Function:		CmosGetRtcStatus
	@Access:		Public
	@Description:
		获取RTC的状态。
	@Parameters:
	@Return:
		返回TRUE则开启，否则关闭中。
*/
BOOL
CmosGetRtcStatus(void)
{
	return rtc_state;
}

/**
	@Function:		CmosEnableRtc
	@Access:		Public
	@Description:
		启用RTC。
	@Parameters:
	@Return:
		返回TRUE则开启成功，否则开启失败。
*/
BOOL
CmosEnableRtc(void)
{
	if(!rtc_state)
	{
		COMMON_LOCK();
		KnlOutByte(0x70, 0x0b);
		uint8 prev = KnlInByte(0x71);
		KnlOutByte(0x70, 0x0b);
		KnlOutByte(0x71, prev | 0x40);
		rtc_state = TRUE;
		COMMON_UNLOCK();
	}
	return TRUE;
}

/**
	@Function:		CmosDisableRtc
	@Access:		Public
	@Description:
		禁用RTC。
	@Parameters:
	@Return:
		返回TRUE则禁用成功，否则禁用失败。
*/
BOOL
CmosDisableRtc(void)
{
	if(rtc_state)
	{
		COMMON_LOCK();
		KnlOutByte(0x70, 0x0b);
		uint8 prev = KnlInByte(0x71);
		KnlOutByte(0x70, 0x0b);
		KnlOutByte(0x71, prev & ~0x40);
		rtc_state = FALSE;
		COMMON_UNLOCK();
	}
	return TRUE;
}

/**
	@Function:		CmosSetRtcRate
	@Access:		Public
	@Description:
		设置RTC的频率。
	@Parameters:
	@Return:
		返回TRUE则设置成功，否则设置失败。
*/
BOOL
CmosSetRtcRate(uint8 rate)
{
	if(rate <= 2 || rate > 15)
		return FALSE;
	COMMON_LOCK();
	KnlOutByte(0x70, 0x0a);
	uint8 prev = KnlInByte(0x71);
	KnlOutByte(0x70, 0x0a);
	KnlOutByte(0x71, (prev & 0xf0) | rate);
	COMMON_UNLOCK();
	return TRUE;
}

/**
	@Function:		CmosEndOfRtc
	@Access:		Public
	@Description:
		IRQ8处理程序结束时需要调用该函数，否则IRQ8不会再次引发。
	@Parameters:
	@Return:
*/
void
CmosEndOfRtc(void)
{
	if(rtc_state)
	{
		COMMON_LOCK();
		KnlOutByte(0x70, 0x0c);
		KnlInByte(0x71);
		COMMON_UNLOCK();
	}
}

/**
	@Function:		CmosEnableNmi
	@Access:		Public
	@Description:
		启用NMI。
	@Parameters:
	@Return:
*/
void
CmosEnableNmi(void)
{
	KnlOutByte(0x70, KnlInByte(0x70) & 0x7f);
}

/**
	@Function:		CmosDisableNmi
	@Access:		Public
	@Description:
		禁用NMI。
	@Parameters:
	@Return:
*/
void
CmosDisableNmi(void)
{
	KnlOutByte(0x70, KnlInByte(0x70) | 0x80);
}
