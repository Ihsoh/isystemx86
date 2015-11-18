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

#define	INT8_SPARAM(sp)		(*((char *)&(sp)))
#define	INT16_SPARAM(sp)	(*((short *)&(sp)))
#define	INT32_SPARAM(sp)	(*((int *)&(sp)))

#define	UINT8_SPARAM(sp)	((uchar)sp)
#define	UINT16_SPARAM(sp)	((ushort)sp)
#define	UINT32_SPARAM(sp)	((uint)sp)

#define	BOOL_SPARAM(sp)		(*((BOOL *)&(sp)))

#define	INT8_PTR_SPARAM(sp)		((char *)sp)
#define	INT16_PTR_SPARAM(sp)	((short *)sp)
#define	INT32_PTR_SPARAM(sp)	((int *)sp)

#define	UINT8_PTR_SPARAM(sp)	((uchar *)sp)
#define	UINT16_PTR_SPARAM(sp)	((ushort *)sp)
#define	UINT32_PTR_SPARAM(sp)	((uint *)sp)

#define	VOID_PTR_SPARAM(sp)		((void *)sp)

#define	SPARAM(p)	(*((uint *)&(p)))

extern void system_call(uint func, uint sub_func, struct SParams * sparams);

#endif
