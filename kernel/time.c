/**
	@File:			time.c
	@Author:		Ihsoh
	@Date:			2017-5-29
	@Description:
		提供与时间相关的功能。
*/

#include "time.h"

#define	_FOUR_YEARS				(365 + 365 + 365 + 366)
#define	_SECONDS_PER_MINUTE		60
#define	_SECONDS_PER_HOUR		(60 * _SECONDS_PER_MINUTE)
#define	_SECONDS_PER_DAY		(24 * _SECONDS_PER_HOUR)

/**
	@Function:		KnlGetDateTimeByUnixTimestamp
	@Access:		Public
	@Description:
		通过Unix时间戳填充DateTime实例。
	@Parameters:
		dt, DateTimePtr, OUT
			指向DateTime实例。
		timestamp, int32, IN
			Unix时间戳，UTC+00:00，单位：秒。
		offset, int32, IN
			偏移，单位：秒。
			如果时区为UTC+08:00，则应该传入28800（8 * 60 * 60）。
			如果时区为UTC-08:00，则应该传入-28800（-8 * 60 * 60）。
	@Return:
		BOOL
			返回TRUE则获取成功，否则失败。
*/
BOOL
KnlGetDateTimeByUnixTimestamp(
	OUT DateTimePtr dt,
	IN int32 timestamp,
	IN int32 offset)
{
	if (dt == NULL)
	{
		return FALSE;
	}

	dt->timestamp = timestamp;
	dt->offset = offset;
	timestamp += offset;

	// 计算年份。
	int32 ndays = timestamp / _SECONDS_PER_DAY + 1;
	int32 n4years = ndays / _FOUR_YEARS;
	int32 nremain = ndays % _FOUR_YEARS;
	int32 year = 1970 + n4years * 4;
	if (nremain < 365)
	{
		dt->d.leap = FALSE;
	}
	else if (nremain < (365 + 365))
	{
		dt->d.leap = FALSE;
		year += 1;
		nremain -= 365;
	}
	else if (nremain < (365 + 365 + 365))
	{
		dt->d.leap = FALSE;
		year += 2;
		nremain -= 365 + 365;
	}
	else
	{
		dt->d.leap = TRUE;
		year += 3;
		nremain -= 365 + 365 + 365;
	}
	dt->d.year = (uint16) year;

	// 计算月和日。
	int32 month = 0;
	int32 day = 0;
	const int32 MONTHS[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	const int32 MONTHS_LEAP[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int32 * m = dt->d.leap ? MONTHS_LEAP : MONTHS;
	int32 i;
	for (i = 0; i < 12; i++)
	{
		int32 ntemp = nremain - m[i];
		if (ntemp <= 0)
		{
			month = i + 1;
			if (ntemp == 0)
			{
				day = m[i];
			}
			else
			{
				day = nremain;
			}
			break;
		}
		nremain = ntemp;
	}
	dt->d.month = (uint8) month;
	dt->d.day = (uint8) day;

	// 计算星期。
	const int32 DELTA[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
	int32 prev_year = year - 1;
	int32 week = day - 1 + year + DELTA[month - 1] + prev_year / 4 - prev_year / 100 + prev_year / 400;
	if (month > 2 && ((year % 4 == 0 || year % 400 == 0) && year % 100 != 0))
	{
		week++;
	}
	week %= 7;
	dt->d.week = (uint8) week;

	// 计算时间。
	int32 temp = timestamp % _SECONDS_PER_DAY;

	int32 hour = temp / _SECONDS_PER_HOUR;
	dt->t.hour = (uint8) hour;
	temp -= hour * _SECONDS_PER_HOUR;

	int32 minute = temp / _SECONDS_PER_MINUTE;
	dt->t.minute = (uint8) minute;
	temp -= minute * _SECONDS_PER_MINUTE;

	dt->t.second = (uint8) temp;

	return TRUE;
}

int32
KnlGetUnixTimestampByDateTime(
	IN DateTimePtr dt)
{
	
}


