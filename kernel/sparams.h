//Filename:		sparams.h
//Author:		Ihsoh
//Date:			2014-7-26
//Descriptor:	System parameters

#ifndef	_SPARAMS_H_
#define	_SPARAMS_H_

#include "types.h"

struct SParams
{
	uint32	param0;
	uint32	param1;
	uint32	param2;
	uint32	param3;
	uint32	param4;
	uint32	param5;
	uint32	param6;
	uint32	param7;
	uint32	param8;
	uint32	param9;
	int32	tid;
};

#define	INT8_SPARAM(sp)		(*((int8 *)&(sp)))
#define	INT16_SPARAM(sp)	(*((int16 *)&(sp)))
#define	INT32_SPARAM(sp)	(*((int32 *)&(sp)))

#define	UINT8_SPARAM(sp)	((uint8)sp)
#define	UINT16_SPARAM(sp)	((uint16)sp)
#define	UINT32_SPARAM(sp)	((uint32)sp)

#define	INT8_PTR_SPARAM(sp)		((int8 *)sp)
#define	INT16_PTR_SPARAM(sp)	((int16 *)sp)
#define	INT32_PTR_SPARAM(sp)	((int32 *)sp)

#define	UINT8_PTR_SPARAM(sp)	((uint8 *)sp)
#define	UINT16_PTR_SPARAM(sp)	((uint16 *)sp)
#define	UINT32_PTR_SPARAM(sp)	((uint32 *)sp)

#define	VOID_PTR_SPARAM(sp)		((void *)sp)

#define	SPARAM(p)	((uint)(p))

#endif
