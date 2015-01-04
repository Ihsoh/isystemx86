//Filename:		screen.h
//Author:		Ihsoh
//Date:			2014-1-29
//Descriptor:	Screen

#ifndef	_SCREEN_H_
#define	_SCREEN_H_

#include "types.h"
#include "cmos.h"
#include "enfont.h"
#include "image.h"
#include "window.h"
#include <string.h>

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

extern void flush_char_buffer(void);
extern void no_flush_char_buffer(void);
extern void set_char_color(uint8 cc);
extern uint8 get_char_color(void);
extern void set_cursor_color(uint8 cc);
extern uint8 get_cursor_color(void);

extern void flush_screen(void);

extern void lock_cursor(void);
extern void unlock_cursor(void);
extern void set_cursor(uint16 x, uint16 y);
extern void set_cursor_pos(uint16 x, uint16 y);
extern void get_cursor(uint16 * x, uint16 * y);
extern void screen_up(void);
extern void print_char_p(int8 chr, uint8 p);
extern void print_char(int8 chr);
extern void print_str(const int8 * str);
extern void print_str_p(const int8 * str, uint8 p);
extern void clear_screen(void);
extern void set_char_color(uint8 cc);
extern void init_screen(void);
extern void print_date(struct CMOSDateTime * date);
extern void print_time(struct CMOSDateTime * time);
extern void print_datetime(struct CMOSDateTime * dt);
extern void set_target_screen(struct CommonImage * ts);

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
extern uint32 console_screen_width, console_screen_height;
extern struct CommonImage console_screen_buffer;

#define	WINDOW_STYLE_HIDDEN_BUTTON		0x00000001
#define	WINDOW_STYLE_CLOSE_BUTTON		0x00000002

extern
struct Window *
create_window(	uint32		width,
				uint32		height,
				uint32		bgcolor,
				uint32		style,
				int8 *		title,
				WindowEvent	event);
extern BOOL destroy_window(struct Window * window);
extern struct Window * get_top_window(void);

#endif
