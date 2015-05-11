/**
	@File:			stdio.h
	@Author:		Ihsoh
	@Date:			2014-1-19
	@Description:
*/

#ifndef	_STDIO_H_
#define	_STDIO_H_

#include "types.h"
#include "stdarg.h"

extern int vsprintf_s(char * buffer, uint size, const char * format, va_list va);
extern int sprintf_s(char * buffer, uint size, const char * format, ...);
extern int sprintf(char * buffer, const char * format, ...);
extern int printf(const char * format, ...);
extern int puts(const char * string);
extern int putchar(int ch);

#endif
