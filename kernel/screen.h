/**
	@File:			screen.h
	@Author:		Ihsoh
	@Date:			2014-1-29
	@Description:
*/

#ifndef	_SCREEN_H_
#define	_SCREEN_H_

#include "types.h"
#include "cmos.h"
#include "enfont.h"
#include "image.h"
#include "window.h"

#include <ilib/string.h>

#define	COLUMN		80
#define	ROW			25

#define	CURSOR_HEIGHT	2

#define	CC_BLACK		0
#define	CC_BLUE			1
#define	CC_GREEN		2
#define	CC_DARKBLUE		3
#define	CC_RED			4
#define	CC_MAGENTA		5
#define	CC_BROWN		6
#define	CC_GRAYWHITE	7
#define	CC_LIGHTGRAY	8
#define	CC_LIGHTBLUE	9
#define	CC_LIGHTGREEN	10
#define	CC_LIGHTCYAN	11
#define	CC_LIGHTRED		12
#define	CC_LIGHTMAGENTA	13
#define	CC_YELLOW		14
#define	CC_WHITE		15
#define	CBGC_BLACK		(CC_BLACK << 4)
#define	CBGC_BLUE		(CC_BLUE << 4)
#define	CGBC_GREEN		(CC_GREEN << 4)
#define	CBGC_DARKBLUE	(CC_DARKBLUE << 4)
#define	CBGC_RED		(CC_RED << 4)
#define	CBGC_MAGEBTA	(CC_MAGENTA << 4)
#define	CBGC_BROWN		(CC_BROWN << 4)
#define	CBGC_GRAYWHITE	(CC_GRAYWHITE << 4)

extern
BOOL
screen_write_console_buffer(IN const uint8 * buffer,
							IN uint32 size);

extern
void
flush_char_buffer(void);

extern
void
no_flush_char_buffer(void);

extern
void
set_char_color(IN uint8 cc);

extern
uint8
get_char_color(void);

extern
void
set_cursor_color(IN uint8 cc);

extern
uint8
get_cursor_color(void);

extern
void
flush_screen(void);

extern
void
lock_cursor(void);

extern
void
unlock_cursor(void);

extern
void
set_cursor(	IN uint16 x,
			IN uint16 y);

extern
void
set_cursor_pos(	IN uint16 x,
				IN uint16 y);

extern
void
get_cursor(	OUT uint16 * x,
			OUT uint16 * y);

extern
void
screen_up(void);

extern
void
print_char_p(	IN int8 chr,
				IN uint8 p);

extern
void
print_char(IN int8 chr);

extern
void
print_str(IN const int8 * str);

extern
void
print_str_p(IN const int8 * str,
			IN uint8 p);

extern 
void
print_err_char_p(	IN int8 chr,
					IN uint8 p);

extern
void
print_err_char(IN int8 chr);

extern
void
print_err_str(IN const int8 * str);

extern
void
print_err_str_p(IN const int8 * str,
				IN uint8 p);

extern
void
clear_screen(void);

extern
void
set_char_color(IN uint8 cc);

extern
void
init_screen(void);

extern
void
print_date(IN struct CMOSDateTime * date);

extern
void
print_time(IN struct CMOSDateTime * time);

extern
void
print_datetime(IN struct CMOSDateTime * dt);

extern
void
set_target_screen(IN struct CommonImage * ts);

#define BUFFER_SIZE	100
#define	printun(n) 	{\
						int8 __buffer__[BUFFER_SIZE];\
						print_str(ultos(__buffer__, (n)));\
					}
#define	printn(n) 	{\
						int8 __buffer__[BUFFER_SIZE];\
						print_str(ltos(__buffer__, (n)));\
					}

#define	printd(n) 	{\
						int8 __buffer__[BUFFER_SIZE];\
						print_str(dtos(__buffer__, (n)));\
					}

#define	printuchex(n)	{\
							int8 __buffer__[BUFFER_SIZE];\
							print_str(uctohexs(__buffer__, (n)));\
						}
#define	printushex(n)	{\
							int8 __buffer__[BUFFER_SIZE];\
							print_str(ustohexs(__buffer__, (n)));\
						}
#define	printuihex(n)	{\
							int8 __buffer__[BUFFER_SIZE];\
							print_str(uitohexs(__buffer__, (n)));\
						}
#define	printulhex(n)	{\
							int8 __buffer__[BUFFER_SIZE];\
							print_str(ultohexs(__buffer__, (n)));\
						}
			
extern struct CommonImage * target_screen;


//Window Mode

#define	MAX_WINDOW_COUNT	256

extern uint32 console_screen_width, console_screen_height;
extern struct CommonImage console_screen_buffer;

extern
void
switch_window(void);

extern
BOOL
set_top_window(IN WindowPtr window);

extern
struct Window *
create_window(	IN uint32		width,
				IN uint32		height,
				IN uint32		bgcolor,
				IN uint32		style,
				IN int8 *		title,
				IN WindowEvent	event);

extern
BOOL
destroy_window(IN struct Window * window);

extern
struct Window *
get_top_window(void);

extern
WindowPtr *
get_windows(OUT uint32 * count);

#endif
