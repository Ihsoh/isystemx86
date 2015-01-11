/**
	@File:			stdarg.h
	@Author:		Ihsoh
	@Date:			2014-1-19
	@Description:
		提供函数变参相关功能。
*/

#include "ilib.h"

typedef unsigned char * va_list;

#define	va_start(vl, a) (vl = ((unsigned char *)&a) + sizeof(a))

#define	va_arg(vl, t) (*((vl += sizeof(t)) - sizeof(t)))

#define	va_end(vl)	(vl = NULL)
