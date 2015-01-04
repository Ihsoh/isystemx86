//Filename:		system.h
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	System functions

#ifndef	_SYSTEM_H_
#define	_SYSTEM_H_

#include <types.h>

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

#define	SCALL_SYSTEM	3

#define	SCALL_EXIT			0
#define	SCALL_ALLOC_M		1
#define	SCALL_FREE_M		2
#define	SCALL_GET_PARAM		3
#define	SCALL_GET_DATETIME	4
#define	SCALL_SET_CLOCK		5

extern void app_exit(void);
extern void * allocm(uint len);
extern int freem(void * ptr);
extern void get_param(char * buffer);
extern int get_param_w(char * buffer);
extern void get_datetime(struct CMOSDateTime * dt);
extern void set_clock(int enable);

#endif

