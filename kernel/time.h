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
int32
KnlGetUnixTimestampByDateTime(
	IN DateTimePtr dt);

#endif
