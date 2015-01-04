//Filename:		cmos.h
//Author:		Ihsoh
//Date:			2014-7-24
//Descriptor:	Emulate kernel CMOS
#ifndef	_CMOS_H_
#define	_CMOS_H_

#include "types.h"

struct CMOSDateTime
{
	ushort year;
	uchar month;
	uchar day;
	uchar day_of_week;
	uchar hour;
	uchar minute;
	uchar second;
} __attribute__((packed));

#endif

