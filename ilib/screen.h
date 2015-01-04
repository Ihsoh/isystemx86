//Filename:		screen.h
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	Screen functions

#ifndef	_SCREEN_H_
#define	_SCREEN_H_

#include <types.h>
#include <image.h>
#include <system.h>

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
#define	SCALL_ENABLE_FLUSH_SCREEN	15
#define	SCALL_DISABLE_FLUSH_SCREEN	16
#define	SCALL_SET_TARGET_SCREEN		17

#define	SCALL_CREATE_WINDOW			18
#define	SCALL_DESTROY_WINDOW		19
#define	SCALL_SHOW_WINDOW			20
#define	SCALL_HIDE_WINDOW			21
#define	SCALL_WINDOW_GET_KEY		22
#define	SCALL_DRAW_WINDOW			23


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
extern void print_char_p(char chr, uchar p);
extern void print_str(char * str);
extern void print_str_p(char * str, uchar p);
extern void clear_screen(void);
extern void set_cursor(ushort x, ushort y);
extern void get_cursor(ushort * x, ushort * y);
extern void print_date(struct CMOSDateTime * date);
extern void print_time(struct CMOSDateTime * time);
extern void print_datetime(struct CMOSDateTime * dt);
extern int vesa_is_valid(void);
extern void get_screen_size(uint * width, uint * height);
extern void set_screen_pixel(uint x, uint y, uint color);
extern uint get_screen_pixel(uint x, uint y);
extern void draw_screen_image(uint x, uint y, uint width, uint height, uint * image);
extern void get_text_screen_size(uint * column, uint * row);
extern void lock_cursor(void);
extern void unlock_cursor(void);
extern void enable_flush_screen(void);
extern void disable_flush_screen(void);
extern void set_target_screen(struct CommonImage * ts);


#define BUFFER_SIZE	100
#define	printun(n) 	{\
						char __buffer__[BUFFER_SIZE];\
						print_str(ultos(__buffer__, (n)));\
					}
#define	printn(n) 	{\
						char __buffer__[BUFFER_SIZE];\
						print_str(ltos(__buffer__, (n)));\
					}

#define	printd(n) 	{\
						char __buffer__[BUFFER_SIZE];\
						print_str(dtos(__buffer__, (n)));\
					}

#define	prints(n) 	{\
						char __buffer__[BUFFER_SIZE];\
						print_str(ftos(__buffer__, (n)));\
					}

#define	printuchex(n)	{\
							char __buffer__[BUFFER_SIZE];\
							print_str(uctohexs(__buffer__, (n)));\
						}
#define	printushex(n)	{\
							char __buffer__[BUFFER_SIZE];\
							print_str(ustohexs(__buffer__, (n)));\
						}
#define	printuihex(n)	{\
							char __buffer__[BUFFER_SIZE];\
							print_str(uitohexs(__buffer__, (n)));\
						}
#define	printulhex(n)	{\
							char __buffer__[BUFFER_SIZE];\
							print_str(ultohexs(__buffer__, (n)));\
						}

#define	WINDOW_STYLE_HIDDEN_BUTTON		0x00000001
#define	WINDOW_STYLE_CLOSE_BUTTON		0x00000002

extern 
uint32
create_window(	uint32		width,
				uint32		height,
				uint32		bgcolor,
				uint32		style,
				int8 *		title);
extern BOOL destroy_window(uint32 id);
extern void show_window(uint32 id);
extern void hide_window(uint32 id);
extern uint8 window_get_key(uint32 id);
extern BOOL draw_window(uint32 id, struct CommonImage * image);

#endif
