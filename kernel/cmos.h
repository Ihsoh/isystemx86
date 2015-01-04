/**
	@File:			cmos.h
	@Author:		Ihsoh
	@Date:			2014-7-17
	@Description:
*/

#ifndef	_CMOS_H_
#define	_CMOS_H_

#include "types.h"

struct CMOSDateTime
{
	uint16	year;
	uint8	month;
	uint8	day;
	uint8	day_of_week;
	uint8	hour;
	uint8	minute;
	uint8	second;
} __attribute__((packed));

extern 
void
get_cmos_date_time(OUT struct CMOSDateTime * dt);

#endif
