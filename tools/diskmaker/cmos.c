//Filename:		cmos.c
//Author:		Ihsoh
//Date:			2014-7-24
//Descriptor:	Emulate kernel CMOS

#include "cmos.h"
#include "time.h"

void get_cmos_date_time(struct CMOSDateTime * dt)
{
	time_t raw_time;
	struct tm * time_info;
	raw_time = time(NULL);
	time_info = localtime(&raw_time);
	dt->year = time_info->tm_year + 1900;
	dt->month = time_info->tm_mon + 1;
	dt->day = time_info->tm_mday;
	dt->day_of_week = time_info->tm_wday + 1;
	dt->hour = time_info->tm_hour;
	dt->minute = time_info->tm_min;
	dt->second = time_info->tm_sec;
}

