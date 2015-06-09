/**
	@File:			stdarg.h
	@Author:		Ihsoh
	@Date:			2014-1-19
	@Description:
		提供函数变参相关功能。
*/

#ifndef	_STDARG_H_
#define	_STDARG_H_

#include "types.h"

typedef unsigned char * va_list;

#define	_INTSIZEOF(t)	(sizeof(t) <= 4 ? 4 : 8)

#define	va_start(vl, a) (vl = ((unsigned char *)&a) + _INTSIZEOF(a))

#define	va_arg(vl, t) (*(t *)((vl += _INTSIZEOF(t)) - _INTSIZEOF(t)))

#define	va_end(vl)	(vl = NULL)

#endif
