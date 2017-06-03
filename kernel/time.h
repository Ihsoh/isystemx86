/**
	@File:			time.h
	@Author:		Ihsoh
	@Date:			2017-5-29
	@Description:
		提供与时间相关的功能。
*/

#ifndef	_TIME_H_
#define	_TIME_H_

#include "types.h"

#define	WEEK_SUNDAY			0
#define	WEEK_MONDAY			1
#define	WEEK_TUESDAY		2
#define	WEEK_WEDNESDAY		3
#define	WEEK_THURSDAY		4
#define	WEEK_FRIDAY			5
#define	WEEK_SATURDAY		6

#define	MONTH_JANUARY		1
#define	MONTH_FEBRUARY		2
#define	MONTH_MARCH			3
#define	MONTH_APRIL			4
#define	MONTH_MAY			5
#define	MONTH_JUNE			6
#define	MONTH_JULY			7
#define	MONTH_AUGUEST		8
#define	MONTH_SEPTEMBER		9
#define	MONTH_OCTOBER		10
#define	MONTH_NOVEMBER		11
#define	MONTH_DECEMBER		12

typedef struct
{
	uint16		year;
	uint8		month;
	uint8		day;
	BOOL		leap;
	uint8		week;
} Date, * DatePtr;

typedef struct
{
	uint8		hour;
	uint8		minute;
	uint8		second;
} Time, * TimePtr;

typedef struct
{
	int32	timestamp;
	int32	offset;
	Date	d;
	Time	t;
} DateTime, * DateTimePtr;

extern
BOOL
KnlGetDateTimeByUnixTimestamp(
	OUT DateTimePtr dt,
	IN int32 timestamp,
	IN int32 offset);

extern
BOOL
KnlGetDateTime(
	OUT DateTimePtr dt,
	IN int32 offset,
	IN int32 year,
	IN int32 month,
	IN int32 day,
	IN int32 hour,
	IN int32 minute,
	IN int32 second);

#endif
