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
BOOL
get_cmos_date_time(OUT struct CMOSDateTime * dt);

extern
BOOL
set_cmos_date_time(IN struct CMOSDateTime * dt);

extern
BOOL
set_cmos_date(	IN uint16 year,
				IN uint8 month,
				IN uint8 day);

extern
BOOL
set_cmos_time(	IN uint8 hour,
				IN uint8 minute,
				IN uint8 second);

extern
BOOL
get_rtc_state(void);

extern
BOOL
enable_rtc(void);

extern
BOOL
disable_rtc(void);

extern
BOOL
set_rtc_rate(uint8 rate);

extern
void
end_of_rtc(void);

extern
void
enable_nmi(void);

extern
void
disable_nmi(void);

#endif
