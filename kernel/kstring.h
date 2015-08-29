/**
	@File:			kstring.h
	@Author:		Ihsoh
	@Date:			2015-2-10
	@Description:
*/

#ifndef	_KSTRING_H_
#define	_KSTRING_H_

#include "types.h"

extern
int8 *
split_string(	OUT int8 * dest, 
				IN const int8 * src,
				IN int8 spt,
				IN int32 max);

typedef uint8 * va_list;

#define	_INTSIZEOF(t)	(sizeof(t) <= 4 ? 4 : 8)

#define	va_start(vl, a) (vl = ((uint8 *)&a) + _INTSIZEOF(a))

#define	va_arg(vl, t) (*(t *)((vl += _INTSIZEOF(t)) - _INTSIZEOF(t)))

#define	va_end(vl)	(vl = NULL)

extern
int32
vsprintf_s(	OUT int8 * buffer,
			IN uint32 size,
			IN const int8 * format,
			IN va_list va);

extern
int32
sprintf_s(	OUT int8 * buffer,
			IN uint32 size,
			IN const int8 * format,
			...);

extern
int32
sprintf(OUT int8 * buffer,
		IN const int8 * format,
		...);

#endif
