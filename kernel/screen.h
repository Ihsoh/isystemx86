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
ScrWriteConsoleBuffer(IN const uint8 * buffer,
							IN uint32 size);

extern
void
ScrEnableFlushConsoleBuffer(void);

extern
void
ScrDisableFlushConsoleBuffer(void);

extern
void
ScrSetConsoleCharColor(IN uint8 cc);

extern
uint8
ScrGetConsoleCharColor(void);

extern
void
ScrSetConsoleCursorColor(IN uint8 cc);

extern
uint8
ScrGetConsoleCursorColor(void);

extern
BOOL
ScrRenderTextBufferEx(	IN OUT ImagePtr image,
						IN uint32 start_x,
						IN uint32 start_y,
						IN uint8 * txtbuf,
						IN uint32 row,
						IN uint32 column,
						IN uint32 curx,
						IN uint32 cury);

extern
BOOL
ScrRenderTextBuffer(IN OUT ImagePtr image,
					IN uint8 * txtbuf,
					IN uint32 row,
					IN uint32 column,
					IN uint32 curx,
					IN uint32 cury);

extern
void
ScrFlushScreen(void);

extern
void
ScrLockConsoleCursor(void);

extern
void
ScrUnlockConsoleCursor(void);

extern
void
ScrSetConsoleCursor(IN uint16 x,
					IN uint16 y);

extern
void
ScrSetConsoleCursorPosition(IN uint16 x,
							IN uint16 y);

extern
void
ScrGetConsoleCursor(OUT uint16 * x,
					OUT uint16 * y);

extern
void
ScrScreenUp(void);

extern
void
ScrPrintCharWithProperty(	IN int8 chr,
							IN uint8 p);

extern
void
ScrPrintChar(IN int8 chr);

extern
void
ScrPrintString(IN const int8 * str);

extern
void
ScrPrintStringWithProperty(	IN const int8 * str,
							IN uint8 p);

extern 
void
ScrPrintCharToStderrWithProperty(	IN int8 chr,
									IN uint8 p);

extern
void
ScrPrintCharToStderr(IN int8 chr);

extern
void
ScrPrintStringToStderr(IN const int8 * str);

extern
void
ScrPrintStringToStderrWithProperty(	IN const int8 * str,
									IN uint8 p);

extern
void
ScrClearScreen(void);

extern
void
ScrSetConsoleCharColor(IN uint8 cc);

extern
void
ScrInit(void);

extern
void
ScrPrintDate(IN struct CMOSDateTime * date);

extern
void
ScrPrintTime(IN struct CMOSDateTime * time);

extern
void
ScrPrintDateTime(IN struct CMOSDateTime * dt);

extern
void
ScrSetTargetScreen(IN struct CommonImage * ts);

#define BUFFER_SIZE	100
#define	printun(n) 	{\
						int8 __buffer__[BUFFER_SIZE];\
						ScrPrintString(ultos(__buffer__, (n)));\
					}
#define	printn(n) 	{\
						int8 __buffer__[BUFFER_SIZE];\
						ScrPrintString(ltos(__buffer__, (n)));\
					}

#define	printd(n) 	{\
						int8 __buffer__[BUFFER_SIZE];\
						ScrPrintString(dtos(__buffer__, (n)));\
					}

#define	printuchex(n)	{\
							int8 __buffer__[BUFFER_SIZE];\
							ScrPrintString(uctohexs(__buffer__, (n)));\
						}
#define	printushex(n)	{\
							int8 __buffer__[BUFFER_SIZE];\
							ScrPrintString(ustohexs(__buffer__, (n)));\
						}
#define	printuihex(n)	{\
							int8 __buffer__[BUFFER_SIZE];\
							ScrPrintString(uitohexs(__buffer__, (n)));\
						}
#define	printulhex(n)	{\
							int8 __buffer__[BUFFER_SIZE];\
							ScrPrintString(ultohexs(__buffer__, (n)));\
						}
			
extern struct CommonImage * target_screen;


//Window Mode

#define	MAX_WINDOW_COUNT	256

extern uint32 console_screen_width, console_screen_height;
extern struct CommonImage console_screen_buffer;

extern
void
ScrSwitchWindow(void);

extern
BOOL
ScrSetTopWindow(IN WindowPtr window);

extern
struct Window *
ScrCreateWindow(	IN uint32		width,
				IN uint32		height,
				IN uint32		bgcolor,
				IN uint32		style,
				IN int8 *		title,
				IN WindowEvent	event);

extern
BOOL
ScrDestroyWindow(IN struct Window * window);

extern
struct Window *
ScrGetTopWindow(void);

extern
WindowPtr *
ScrGetWindows(OUT uint32 * count);

#endif
