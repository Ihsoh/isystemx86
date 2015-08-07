//Filename:		screen.h
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	Screen functions

#ifndef	_SCREEN_H_
#define	_SCREEN_H_

#include "types.h"
#include "image.h"
#include "system.h"

#define	SCALL_SCREEN		0

#define	SCALL_PRINT_CHAR			0
#define	SCALL_PRINT_CHAR_P			1
#define	SCALL_PRINT_STR				2
#define	SCALL_PRINT_STR_P			3
#define	SCALL_CLEAR_SCREEN			4
#define	SCALL_SET_CURSOR			5
#define	SCALL_GET_CURSOR			6	
#define	SCALL_VESA_IS_VALID			7
#define	SCALL_GET_SCREEN_SIZE		8
#define	SCALL_SET_SCREEN_PIXEL		9
#define	SCALL_GET_SCREEN_PIXEL		10
#define	SCALL_DRAW_SCREEN_IMAGE		11
#define	SCALL_GET_TEXT_SCREEN_SIZE	12
#define	SCALL_LOCK_CURSOR			13
#define	SCALL_UNLOCK_CURSOR			14
// NOT USED							15
// NOT USED							16
#define	SCALL_SET_TARGET_SCREEN		17

#define	SCALL_CREATE_WINDOW			18
#define	SCALL_DESTROY_WINDOW		19
#define	SCALL_SHOW_WINDOW			20
#define	SCALL_HIDE_WINDOW			21
#define	SCALL_WINDOW_GET_KEY		22
#define	SCALL_DRAW_WINDOW			23

#define	SCALL_PRINT_ERR_CHAR		24
#define	SCALL_PRINT_ERR_CHAR_P		25
#define	SCALL_PRINT_ERR_STR			26
#define	SCALL_PRINT_ERR_STR_P		27

#define	SCALL_FLUSH_SCREEN 			28

#define	SCALL_WRITE_CONSOLE_BUFFER	29

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

extern void print_char(char chr);
#define	il_print_char(chr)	(print_char((chr)))
#define	ILPrintChar(chr)	(print_char((chr)))

extern void print_char_p(char chr, uchar p);
#define	il_print_char_p(chr, p)	(print_char_p((chr), (p)))
#define	ILPrintCharWithProperty(chr, p)	(print_char_p((chr), (p)))

extern void print_str(char * str);
#define	il_print_str(str)	(print_str(str))
#define	ILPrintString(str)	(print_str(str))

extern void print_str_p(char * str, uchar p);
#define	il_print_str_p(str, p)	(print_str_p((str), (p)))
#define	ILPrintStringWithProperty(str, p)	(print_str_p((str), (p)))

extern void clear_screen(void);
#define	il_clear_screen()	(clear_screen())
#define	ILClearScreen()	(clear_screen())

extern void set_cursor(ushort x, ushort y);
#define	il_set_cursor(x, y)	(set_cursor((x), (y)))
#define	ILSetCursor(x, y)	(set_cursor((x), (y)))

extern void get_cursor(ushort * x, ushort * y);
#define	il_get_cursor(x, y)	(get_cursor((x), (y)))
#define	ILGetCursor(x, y)	(get_cursor((x), (y)))

extern void print_date(struct CMOSDateTime * date);
#define	il_print_date(date)	(print_date((date)))
#define	ILPrintDate(date)	(print_date((date)))

extern void print_time(struct CMOSDateTime * time);
#define	il_print_time(time)	(print_time((time)))
#define	ILPrintTime(time)	(print_time((time)))

extern void print_datetime(struct CMOSDateTime * dt);
#define	il_print_datetime(dt)	(print_datetime((dt)))
#define	ILPrintDateTime(dt)	(print_datetime((dt)))

extern int vesa_is_valid(void);
#define	il_vesa_is_valid()	(vesa_is_valid())
#define	ILCheckVesaStatus()	(vesa_is_valid())

extern void get_screen_size(uint * width, uint * height);
#define	il_get_screen_size(width, height)	(get_screen_size((width), (height)))
#define	ILGetScreenSize(width, height)	(get_screen_size((width), (height)))

extern void set_screen_pixel(uint x, uint y, uint color);
#define	il_set_screen_pixel(x, y, color)	(set_screen_pixel((x), (y), (color)))
#define	ILSetScreenPixel(x, y, color)	(set_screen_pixel((x), (y), (color)))

extern uint get_screen_pixel(uint x, uint y);
#define	il_get_screen_pixel(x, y)	(get_screen_pixel((x), (y)))
#define	ILGetScreenPixel(x, y)	(get_screen_pixel((x), (y)))

extern void draw_screen_image(uint x, uint y, uint width, uint height, uint * image);
#define	il_draw_screen_image(x, y, width, height, image)	(draw_screen_image((x), (y), (width), (height), (image)))
#define	ILDrawScreenImage(x, y, width, height, image)	(draw_screen_image((x), (y), (width), (height), (image)))

extern void get_text_screen_size(uint * column, uint * row);
#define	il_get_text_screen_size(column, row)	(get_text_screen_size((column), (row)))
#define	ILGetTextScreenSize(column, row)	(get_text_screen_size((column), (row)))

extern void lock_cursor(void);
#define	il_lock_cursor()	(lock_cursor())
#define	ILLockCursor()	(lock_cursor())

extern void unlock_cursor(void);
#define	il_unlock_cursor()	(unlock_cursor())
#define	ILUnlockCursor()	(unlock_cursor())

extern void enable_flush_screen(void);
#define	il_enable_flush_screen()	(enable_flush_screen())
#define	ILEnableFlushScreen()	(enable_flush_screen())

extern void disable_flush_screen(void);
#define	il_disable_flush_screen()	(disable_flush_screen())
#define	ILDisableFlushScreen()	(disable_flush_screen())

extern void set_target_screen(struct CommonImage * ts);
#define	il_set_target_screen(ts)	(set_target_screen((ts)))
#define	ILSetTargetScreen(ts)	(set_target_screen((ts)))

extern void print_err_char(char chr);
#define	il_print_err_char(chr)	(print_err_char((chr)))
#define	ILPrintErrorChar(chr)	(print_err_char((chr)))

extern void print_err_char_p(char chr, uchar p);
#define	il_print_err_char_p(chr, p)	(print_err_char_p((chr), (p)))
#define	ILPrintErrorCharWithProperty(chr, p)	(print_err_char_p((chr), (p)))

extern void print_err_str(char * str);
#define	il_print_err_str(str)	(print_err_str(str))
#define	ILPrintErrorString(str)	(print_err_str(str))

extern void print_err_str_p(char * str, uchar p);
#define	il_print_err_str_p(str, p)	(print_err_str_p((str), (p)))
#define	ILPrintErrorStringWithProperty(str, p)	(print_err_str_p((str), (p)))

extern void flush_screen(void);
#define	il_flush_screen()	(flush_screen())
#define	ILFlushScreen()	(flush_screen())

extern int write_console_buffer(uint8 * buffer, uint32 size);
#define	il_write_console_buffer(buffer, size)	(write_console_buffer((buffer), (size)))
#define	ILWriteConsoleBuffer(buffer, size)	(write_console_buffer((buffer), (size)))


#define BUFFER_SIZE	100
#define	printun(n) 	{\
						char __buffer__[BUFFER_SIZE];\
						print_str(ultos(__buffer__, (n)));\
					}
#define ILPrintUInt(n)	(printun((n)))

#define	printn(n) 	{\
						char __buffer__[BUFFER_SIZE];\
						print_str(ltos(__buffer__, (n)));\
					}
#define	ILPrintInt(n)	(printn((n)))

#define	printd(n) 	{\
						char __buffer__[BUFFER_SIZE];\
						print_str(dtos(__buffer__, (n)));\
					}
#define	ILPrintDouble(n)	(printd((n)))

#define	prints(n) 	{\
						char __buffer__[BUFFER_SIZE];\
						print_str(ftos(__buffer__, (n)));\
					}
#define	ILPrintFloat(n)	(prints((n)))

#define	printuchex(n)	{\
							char __buffer__[BUFFER_SIZE];\
							print_str(uctohexs(__buffer__, (n)));\
						}
#define	ILPrintUCharHex(n)	(printuchex((n)))

#define	printushex(n)	{\
							char __buffer__[BUFFER_SIZE];\
							print_str(ustohexs(__buffer__, (n)));\
						}
#define	ILPrintUShortHex(n)	(printushex((n)))

#define	printuihex(n)	{\
							char __buffer__[BUFFER_SIZE];\
							print_str(uitohexs(__buffer__, (n)));\
						}
#define	ILPrintUIntHex(n)	(printuihex((n)))

#define	printulhex(n)	{\
							char __buffer__[BUFFER_SIZE];\
							print_str(ultohexs(__buffer__, (n)));\
						}
#define	ILPrintULongHex(n)	(printulhex((n)))

#define	WINDOW_STYLE_HIDDEN_BUTTON		0x00000001
#define	WINDOW_STYLE_CLOSE_BUTTON		0x00000002

extern 
uint32
create_window(	uint32		width,
				uint32		height,
				uint32		bgcolor,
				uint32		style,
				int8 *		title);
#define	il_create_window(width, height, bgcolor, style, title)	create_window((width), (height), (bgcolor), (style), (title))
#define	ILCreateWindow(width, height, bgcolor, style, title)	create_window((width), (height), (bgcolor), (style), (title))

extern BOOL destroy_window(uint32 id);
#define	il_destroy_window(id)	(destroy_window((id)))
#define	ILDestroyWindow(id)	(destroy_window((id)))

extern void show_window(uint32 id);
#define	il_show_window(id)	(show_window((id)))
#define	ILShowWindow(id)	(show_window((id)))

extern void hide_window(uint32 id);
#define	il_hide_window(id)	(hide_window((id)))
#define	ILHideWindow(id)	(hide_window((id)))

extern uint8 window_get_key(uint32 id);
#define	il_window_get_key(id)	(window_get_key((id)))
#define	ILGetKeyFromWindow(id)	(window_get_key((id)))

extern BOOL draw_window(uint32 id, struct CommonImage * image);
#define	il_draw_window(id, image)	(draw_window((id), (image)))
#define	ILDrawWindow(id, image)	(draw_window((id), (image)))

#endif
