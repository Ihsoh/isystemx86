//Filename:		sparams.h
//Author:		Ihsoh
//Date:			2014-7-26
//Descriptor:	System parameters

#ifndef	_SPARAMS_H_
#define	_SPARAMS_H_

#include "types.h"

struct SParams
{
	uint	param0;
	uint	param1;
	uint	param2;
	uint	param3;
	uint	param4;
	uint	param5;
	uint	param6;
	uint	param7;
	uint	param8;
	uint	param9;
	int		tid;
};

#define	SPARAM(p)	((uint)p)

extern void system_call(uint func, uint sub_func, struct SParams * sparams);

#endif
