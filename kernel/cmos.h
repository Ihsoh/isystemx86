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
CmosGetDateTime(OUT struct CMOSDateTime * dt);

extern
BOOL
CmosSetDateTime(IN struct CMOSDateTime * dt);

extern
BOOL
CmosSetDate(	IN uint16 year,
				IN uint8 month,
				IN uint8 day);

extern
BOOL
CmosSetTime(	IN uint8 hour,
				IN uint8 minute,
				IN uint8 second);

extern
BOOL
CmosGetRtcStatus(void);

extern
BOOL
CmosEnableRtc(void);

extern
BOOL
CmosDisableRtc(void);

extern
BOOL
CmosSetRtcRate(uint8 rate);

extern
void
CmosEndOfRtc(void);

extern
void
CmosEnableNmi(void);

extern
void
CmosDisableNmi(void);

#endif
