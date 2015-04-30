//Filename:		screen.c
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	Screen functions

#include "screen.h"
#include "types.h"
#include "sparams.h"
#include "system.h"
#include "image.h"

void print_char(char chr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(chr);
	system_call(SCALL_SCREEN, SCALL_PRINT_CHAR, &sparams);
}

void print_char_p(char chr, uchar p)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(chr);
	sparams.param1 = SPARAM(p);
	system_call(SCALL_SCREEN, SCALL_PRINT_CHAR_P, &sparams);
}

void print_str(char * str)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(str);
	system_call(SCALL_SCREEN, SCALL_PRINT_STR, &sparams);
}

void print_str_p(char * str, uchar p)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(str);
	sparams.param1 = SPARAM(p);
	system_call(SCALL_SCREEN, SCALL_PRINT_STR_P, &sparams);
}

void clear_screen(void)
{
	struct SParams sparams;
	system_call(SCALL_SCREEN, SCALL_CLEAR_SCREEN, &sparams);
}

void set_cursor(ushort x, ushort y)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(x);
	sparams.param1 = SPARAM(y);
	system_call(SCALL_SCREEN, SCALL_SET_CURSOR, &sparams);
}

void get_cursor(ushort * x, ushort * y)
{
	struct SParams sparams;
	system_call(SCALL_SCREEN, SCALL_GET_CURSOR, &sparams);
	*x = UINT16_SPARAM(sparams.param0);
	*y = UINT16_SPARAM(sparams.param1);
}

void print_date(struct CMOSDateTime * date)
{
	printn(date->year);
	print_str("-");
	if(date->month < 10)
		print_str("0");
	printn(date->month);
	print_str("-");
	if(date->day < 10)
		print_str("0");
	printn(date->day);
}

void print_time(struct CMOSDateTime * time)
{
	if(time->hour < 10)
		print_str("0");
	printn(time->hour);
	print_str(":");
	if(time->minute < 10)
		print_str("0");
	printn(time->minute);
	print_str(":");
	if(time->second < 10)
		print_str("0");
	printn(time->second);
}

void print_datetime(struct CMOSDateTime * dt)
{
	print_date(dt);
	print_str(" ");
	print_time(dt);
}

int vesa_is_valid(void)
{
	struct SParams sparams;
	system_call(SCALL_SCREEN, SCALL_VESA_IS_VALID, &sparams);
	return INT32_SPARAM(sparams.param0);
}

void get_screen_size(uint * width, uint * height)
{
	struct SParams sparams;
	system_call(SCALL_SCREEN, SCALL_GET_SCREEN_SIZE, &sparams);
	*width = UINT32_SPARAM(sparams.param0);
	*height = UINT32_SPARAM(sparams.param1);
}

void set_screen_pixel(uint x, uint y, uint color)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(x);
	sparams.param1 = SPARAM(y);
	sparams.param2 = SPARAM(color);
	system_call(SCALL_SCREEN, SCALL_SET_SCREEN_PIXEL, &sparams);
}

uint get_screen_pixel(uint x, uint y)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(x);
	sparams.param1 = SPARAM(y);
	system_call(SCALL_SCREEN, SCALL_GET_SCREEN_PIXEL, &sparams);
	return UINT32_SPARAM(sparams.param0);
}

void draw_screen_image(uint x, uint y, uint width, uint height, uint * image)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(x);
	sparams.param1 = SPARAM(y);
	sparams.param2 = SPARAM(width);
	sparams.param3 = SPARAM(height);
	sparams.param4 = SPARAM(image);
	system_call(SCALL_SCREEN, SCALL_DRAW_SCREEN_IMAGE, &sparams);
}

void get_text_screen_size(uint * column, uint * row)
{
	struct SParams sparams;
	system_call(SCALL_SCREEN, SCALL_GET_TEXT_SCREEN_SIZE, &sparams);
	*column = UINT32_SPARAM(sparams.param0);
	*row = UINT32_SPARAM(sparams.param1);
}

void lock_cursor(void)
{
	struct SParams sparams;
	system_call(SCALL_SCREEN, SCALL_LOCK_CURSOR, &sparams);
}

void unlock_cursor(void)
{
	struct SParams sparams;
	system_call(SCALL_SCREEN, SCALL_UNLOCK_CURSOR, &sparams);
}

void enable_flush_screen(void)
{
	struct SParams sparams;
	system_call(SCALL_SCREEN, SCALL_ENABLE_FLUSH_SCREEN, &sparams);
}

void disable_flush_screen(void)
{
	struct SParams sparams;
	system_call(SCALL_SCREEN, SCALL_DISABLE_FLUSH_SCREEN, &sparams);
}

void set_target_screen(struct CommonImage * ts)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(ts);
	system_call(SCALL_SCREEN, SCALL_SET_TARGET_SCREEN, &sparams);
}

uint32
create_window(	uint32		width,
				uint32		height,
				uint32		bgcolor,
				uint32		style,
				int8 *		title)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(width);
	sparams.param1 = SPARAM(height);
	sparams.param2 = SPARAM(bgcolor);
	sparams.param3 = SPARAM(style);
	sparams.param4 = SPARAM(title);
	system_call(SCALL_SCREEN, SCALL_CREATE_WINDOW, &sparams);
	return UINT32_SPARAM(sparams.param0);
}

BOOL
destroy_window(uint32 id)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(id);
	system_call(SCALL_SCREEN, SCALL_DESTROY_WINDOW, &sparams);
	return INT32_SPARAM(sparams.param0);
}

void
show_window(uint32 id)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(id);
	system_call(SCALL_SCREEN, SCALL_SHOW_WINDOW, &sparams);
}

void
hide_window(uint32 id)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(id);
	system_call(SCALL_SCREEN, SCALL_HIDE_WINDOW, &sparams);
}

uint8
window_get_key(uint32 id)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(id);
	system_call(SCALL_SCREEN, SCALL_WINDOW_GET_KEY, &sparams);
	return UINT8_SPARAM(sparams.param0);
}

BOOL
draw_window(uint32 id, struct CommonImage * image)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(id);
	sparams.param1 = SPARAM(image);
	system_call(SCALL_SCREEN, SCALL_DRAW_WINDOW, &sparams);
	return INT32_SPARAM(sparams.param0);
}

void print_err_char(char chr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(chr);
	system_call(SCALL_SCREEN, SCALL_PRINT_ERR_CHAR, &sparams);
}

void print_err_char_p(char chr, uchar p)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(chr);
	sparams.param1 = SPARAM(p);
	system_call(SCALL_SCREEN, SCALL_PRINT_ERR_CHAR_P, &sparams);
}

void print_err_str(char * str)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(str);
	system_call(SCALL_SCREEN, SCALL_PRINT_ERR_STR, &sparams);
}

void print_err_str_p(char * str, uchar p)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(str);
	sparams.param1 = SPARAM(p);
	system_call(SCALL_SCREEN, SCALL_PRINT_ERR_STR_P, &sparams);
}
