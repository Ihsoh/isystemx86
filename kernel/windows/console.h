/**
	@File:			console.h
	@Author:		Ihsoh
	@Date:			2014-12-11
	@Description:
*/

#ifndef	_WINDOWS_CONSOLE_H_
#define	_WINDOWS_CONSOLE_H_

#include "../types.h"
#include "../window.h"

extern struct Window * console_window;

extern
BOOL
WinConsoleInit(void);

extern
void
WinConsoleShow(void);

extern
struct Window *
get_console_window(void);

#endif
