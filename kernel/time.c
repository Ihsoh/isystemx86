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

static const int32 MONTHS[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const int32 MONTHS_LEAP[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
	@Function:		_KnlGetWeek
	@Access:		Private
	@Description:
		通过年月日获取星期（0 ~ 7）。
	@Parameters:
		year, int32, IN
			年份。
		month, int32, IN
			月份。
		day, int32, IN
			日期。
	@Return:
		int32
			星期（0 ~ 6）。
*/
static
int32
_KnlGetWeek(
	IN int32 year,
	IN int32 month,
	IN int32 day)
{
	const int32 DELTA[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
	int32 prev_year = year - 1;
	int32 week = day - 1 + year + DELTA[month - 1] + prev_year / 4 - prev_year / 100 + prev_year / 400;
	if (month > 2 && ((year % 4 == 0 || year % 400 == 0) && year % 100 != 0))
	{
		week++;
	}
	week %= 7;
	return week;
}

/**
	@Function:		KnlGetDateTimeByUnixTimestamp
	@Access:		Public
	@Description:
		通过Unix时间戳填充DateTime实例，不支持负数时间戳。
	@Parameters:
		dt, DateTimePtr, OUT
			指向DateTime实例。
		timestamp, int32, IN
			Unix时间戳，UTC+00:00，单位：秒。
			不支持负数。
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
	if (dt == NULL
		|| timestamp < 0)
	{
		goto err;
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
	dt->d.week = (uint8) _KnlGetWeek(year, month, day);

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
err:
	return FALSE;
}

/**
	@Function:		KnlGetDateTime
	@Access:		Public
	@Description:
		通过年、月、日、时、分、秒获取DateTime实例。
	@Parameters:
		dt, DateTimePtr, OUT
			指向DateTime实例。
		offset, int32, IN
			偏移，单位：秒。
			如果时区为UTC+08:00，则应该传入28800（8 * 60 * 60）。
			如果时区为UTC-08:00，则应该传入-28800（-8 * 60 * 60）。
		year, int32, IN
			年。
		month, int32, IN
			月。
		day, int32, IN
			日。
		hour, int32, IN
			时。
		minute, int32, IN
			分。
		second, int32, IN
			秒。
	@Return:
		BOOL
			返回TRUE则获取成功，否则失败。
*/
BOOL
KnlGetDateTime(
	OUT DateTimePtr dt,
	IN int32 offset,
	IN int32 year,
	IN int32 month,
	IN int32 day,
	IN int32 hour,
	IN int32 minute,
	IN int32 second)
{
	if (dt == NULL
		|| year < 1970 || year >= 2038
		|| month < 1 || month > 12
		|| hour < 0 || hour > 23
		|| minute < 0 || minute > 59
		|| second < 0 || second > 59)
	{
		return FALSE;
	}

	int32 timestamp = -offset;

	timestamp += second;

	timestamp += minute * _SECONDS_PER_MINUTE;

	timestamp += hour * _SECONDS_PER_HOUR;

	timestamp += (day - 1) * _SECONDS_PER_DAY;

	int32 prev_year = year - 1;
	int32 n_leap_year = prev_year / 4 - prev_year / 100 + prev_year / 400;
	int32 n_nonleap_year = prev_year - n_leap_year;
	timestamp += (n_leap_year - 477) * 366 * _SECONDS_PER_DAY;
	timestamp += (n_nonleap_year - 1492) * 365 * _SECONDS_PER_DAY;

	BOOL leap = year % 4 == 0 || (year > 100 && year % 400 == 0);

	int32 * m = leap ? MONTHS_LEAP : MONTHS;
	int32 i;
	for (i = 0; i < month - 1; i++)
	{
		timestamp += m[i] * _SECONDS_PER_DAY;
	}

	int32 week = _KnlGetWeek(year, month, day);

	dt->timestamp = timestamp;
	dt->offset = offset;
	dt->d.year = (uint16) year;
	dt->d.month = (uint8) month;
	dt->d.day = (uint8) day;
	dt->d.leap = leap;
	dt->d.week = (uint8) week;
	dt->t.hour = (uint8) hour;
	dt->t.minute = (uint8) minute;
	dt->t.second = (uint8) second;

	return TRUE;
err:
	return FALSE;
}


