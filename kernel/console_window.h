/**
	@File:			console_window.h
	@Author:		Ihsoh
	@Date:			2014-12-11
	@Description:
*/

#ifndef	_CONSOLE_WINDOW_
#define	_CONSOLE_WINDOW_

#include "types.h"
#include "window.h"

extern struct Window * console_window;

extern
BOOL
init_console_window(void);

extern
void
show_console_window(void);

extern
struct Window *
get_console_window(void);

#endif
