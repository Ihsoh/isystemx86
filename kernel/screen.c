/**
	@File:			screen.c
	@Author:		Ihsoh
	@Date:			2014-1-28
	@Description:
		提供与屏幕显示相关的功能。
*/

#include "types.h"
#include "386.h"
#include "screen.h"
#include "cmos.h"
#include "vesa.h"
#include "enfont.h"
#include "tasks.h"
#include "ifs1fs.h"
#include "system.h"
#include "die.h"
#include "kernel.h"
#include "image.h"
#include "window.h"
#include "clock_window.h"
#include "console_window.h"
#include "sfunc_window.h"
#include "klib.h"
#include "cmlock.h"
#include "bmp/bmp.h"

#include <ilib/string.h>

#define	VIDEO_MEM	0xb8000
#define	BUFFER_SIZE	100

static uint16 cursor_x = 0;
static uint16 cursor_y = 0;
static uint8 char_color = CC_GRAYWHITE | CBGC_BLACK;
static uint8 cursor_color = CC_WHITE;
static int32 lock_csr = 0;

uint32 console_screen_width, console_screen_height;
struct CommonImage console_screen_buffer;
static uint32 screen_width, screen_height;
static struct CommonImage screen_buffer;
static struct CommonImage bg_image;
static struct CommonImage pointer_image;
static uint8 * screen_char_buffer;
static int32 flush_cbuffer = 1;

struct CommonImage * target_screen = NULL;

/*
	============================== 窗体 ==============================
*/
#define	TCOLOR				0xffffffff
#define	MAX_WINDOW_COUNT	32

static struct Window * windows[MAX_WINDOW_COUNT];
static uint32 window_count = 0;
static BOOL move_window = 0;
static BOOL mouse_left_button_down = 0;
static int32 old_mouse_x = 0, old_mouse_y = 0;

/**
	@Function:		flush_char_buffer
	@Access:		Public
	@Description:
		允许刷新字符缓冲区。
	@Parameters:
	@Return:	
*/
void
flush_char_buffer(void)
{
	flush_cbuffer = 1;
}

/**
	@Function:		no_flush_char_buffer
	@Access:		Public
	@Description:
		禁止刷新字符缓冲区。
	@Parameters:
	@Return:	
*/
void
no_flush_char_buffer(void)
{
	flush_cbuffer = 0;
}

/**
	@Function:		set_char_color
	@Access:		Public
	@Description:
		设置字符颜色属性。
	@Parameters:
		cc, uint8, IN
			字符颜色属性。
	@Return:	
*/
void
set_char_color(IN uint8 cc)
{
	char_color = cc;
}

/**
	@Function:		get_char_color
	@Access:		Public
	@Description:
		获取字符颜色属性。
	@Parameters:
	@Return:
		uint8
			字符颜色属性。		
*/
uint8
get_char_color(void)
{
	return char_color;
}

/**
	@Function:		set_cursor_color
	@Access:		Public
	@Description:
		设置光标颜色属性。
	@Parameters:
		cc, uint8, IN
			光标颜色属性。
	@Return:		
*/
void
set_cursor_color(IN uint8 cc)
{
	cursor_color = cc;
}

/**
	@Function:		get_cursor_color
	@Access:		Public
	@Description:
		获取光标颜色属性。
	@Parameters:
	@Return:
		uint8
			光标颜色属性。		
*/
uint8
get_cursor_color(void)
{
	return cursor_color;
}

/**
	@Function:		lock_cursor
	@Access:		Public
	@Description:
		锁定窗体。
	@Parameters:
	@Return:	
*/
void
lock_cursor(void)
{
	lock_csr = 1;
}

/**
	@Function:		unlock_cursor
	@Access:		Public
	@Description:
		解锁窗体。
	@Parameters:
	@Return:	
*/
void
unlock_cursor(void)
{
	lock_csr = 0;
}

/**
	@Function:		set_cursor_pos
	@Access:		Public
	@Description:
		设置光标位置，但不改变在屏幕上的显示位置。
	@Parameters:
		x, uint16, IN
			X 坐标。
		y, uint16, IN
			Y 坐标。
	@Return:	
*/
void
set_cursor_pos(	IN uint16 x,
				IN uint16 y)
{
	cursor_x = x;
	cursor_y = y;
}

/**
	@Function:		set_cursor
	@Access:		Public
	@Description:
		设置光标位置，并且改变在屏幕上的显示位置。
	@Parameters:
		x, uint16, IN
			X 坐标。
		y, uint16, IN
			Y 坐标。
	@Return:	
*/
void
set_cursor(	IN uint16 x,
			IN uint16 y)
{
	set_cursor_pos(x, y);
	if(!lock_csr)
		if(!vesa_is_valid())
		{
			uint16 off = y * COLUMN + x;
			uint8 offl = (uint8)off;
			uint8 offh = (uint8)(off >> 8);
			outb(0x3d4, 0x0e);
			outb(0x3d5, offh);
			outb(0x3d4, 0x0f);
			outb(0x3d5, offl);
		}
}

/**
	@Function:		get_cursor
	@Access:		Public
	@Description:
		获取光标位置。
	@Parameters:
		x, uint16 *, OUT
			X 坐标。
		y, uint16 *, OUT
			Y 坐标。
	@Return:	
*/
void
get_cursor(	OUT uint16 * x,
			OUT uint16 * y)
{
	*x = cursor_x;
	*y = cursor_y;
}

/**
	@Function:		screen_up
	@Access:		Public
	@Description:
		向上滚一行。
	@Parameters:
	@Return:	
*/
void
screen_up(void)
{
	uint8 * dst;
	uint8 * src;
	if(vesa_is_valid())
	{
		dst = (uint8 *)screen_char_buffer;
		src = (uint8 *)screen_char_buffer + 1 * COLUMN * 2 + 0 * 2;
	}
	else
	{
		dst = (uint8 *)VIDEO_MEM;
		src = (uint8 *)VIDEO_MEM + 1 * COLUMN * 2 + 0 * 2;
	}
	int32 i;
	for(i = 0; i < (ROW - 1) * COLUMN * 2; i++)
		*(dst + i) = *(src + i); 
	for(; i < ROW * COLUMN * 2; i++)
		if(i % 2 == 0)
			*(dst + i) = 0;
		else		
			*(dst + i) = char_color;
}

/**
	@Function:		print_char_p_screen
	@Access:		Private
	@Description:
		打印一个字符到屏幕，并且附带字符属性。
	@Parameters:
		chr, int8, IN
			字符。
		p, uint8, IN
			字符颜色属性。
	@Return:	
*/
static
void
print_char_p_screen(IN int8 chr,
					IN uint8 p)
{
	uint8 * off;
	if(chr == '\n')
		if(cursor_y == ROW - 1)
		{
			screen_up();
			set_cursor(0, cursor_y);
		}
		else
			set_cursor(0, cursor_y + 1);
	else if(chr == '\r')
		set_cursor(0, cursor_y);
	else if(chr == '\t')
	{
		uint16 x = ((uint16)(cursor_x / 4) + 1) * 4;
		if(x < COLUMN)
			set_cursor(x, cursor_y);
		else
			set_cursor(COLUMN - 1, cursor_y);
	}
	else
	{	
		if(vesa_is_valid())
			off = (uint8 *)screen_char_buffer + cursor_y * COLUMN * 2 + cursor_x * 2;
		else
			off = (uint8 *)VIDEO_MEM + cursor_y * COLUMN * 2 + cursor_x * 2;
		*off = chr;
		*(off + 1) = p;
		if(cursor_x == COLUMN - 1 && cursor_y == ROW - 1)
		{
			screen_up();
			set_cursor(0, cursor_y);
		}
		else if(cursor_x == COLUMN - 1 && cursor_y != ROW - 1)
			set_cursor(0, cursor_y + 1);
		else
			set_cursor(cursor_x + 1, cursor_y);
	}
}

/**
	@Function:		print_char_p
	@Access:		Public
	@Description:
		打印一个字符到标准输出，并且附带字符属性。
		内核任务的标准输出永远是屏幕。
		用户任务的标准输出则根据struct Task结构体的stdout字段指定。
	@Parameters:
		chr, int8, IN
			字符。
		p, uint8, IN
			字符颜色属性。
	@Return:	
*/
void
print_char_p(	IN int8 chr,
				IN uint8 p)
{
	if(kernel_is_knltask())
		print_char_p_screen(chr, p);
	else
	{
		int32 tid = kernel_get_current_tid();
		struct Task * task = get_task_info_ptr(tid);
		if(task != NULL)
			if(task->stdout == NULL)
				print_char_p_screen(chr, p);
			else
				fappend(task->stdout, &chr, 1);
	}
}

/**
	@Function:		print_char
	@Access:		Public
	@Description:
		打印一个字符到标准输出。
		内核任务的标准输出永远是屏幕。
		用户任务的标准输出则根据struct Task结构体的stdout字段指定。
	@Parameters:
		chr, int8, IN
			字符。
	@Return:	
*/
void
print_char(IN int8 chr)
{
	print_char_p(chr, char_color);
}

/**
	@Function:		print_str
	@Access:		Public
	@Description:
		打印字符串到标准输出。
		内核任务的标准输出永远是屏幕。
		用户任务的标准输出则根据struct Task结构体的stdout字段指定。
	@Parameters:
		str, const int8 *, IN
			字符串。
	@Return:
*/
void
print_str(IN const int8 * str)
{
	if(kernel_is_knltask())
		while(*str != '\0')
			print_char(*(str++));
	else
	{
		int32 tid = kernel_get_current_tid();
		struct Task * task = get_task_info_ptr(tid);
		if(task != NULL)
			if(task->stdout == NULL)
				while(*str != '\0')
					print_char(*(str++));
			else
				fappend(task->stdout, str, strlen(str));
	}
}

/**
	@Function:		print_str_p
	@Access:		Public
	@Description:
		打印字符串到标准输出，并且附带字符属性。
		内核任务的标准输出永远是屏幕。
		用户任务的标准输出则根据struct Task结构体的stdout字段指定。
	@Parameters:
		str, const int8 *, IN
			字符串。
		p, uint8, IN
			字符颜色属性。
	@Return:
*/
void
print_str_p(IN const int8 * str,
			IN uint8 p)
{
	if(kernel_is_knltask())
		while(*str != '\0')
			print_char_p(*(str++), p);
	else
	{
		int32 tid = kernel_get_current_tid();
		struct Task * task = get_task_info_ptr(tid);
		if(task != NULL)
			if(task->stdout == NULL)
				while(*str != '\0')
					print_char_p(*(str++), p);
			else
				fappend(task->stdout, str, strlen(str));
	}
}

/**
	@Function:		print_err_char_p
	@Access:		Public
	@Description:
		打印一个字符到标准错误，并且附带字符属性。
		内核任务的标准错误永远是屏幕。
		用户任务的标准错误则根据struct Task结构体的stderr字段指定。
	@Parameters:
		chr, int8, IN
			字符。
		p, uint8, IN
			字符颜色属性。
	@Return:	
*/
print_err_char_p(	IN int8 chr,
					IN uint8 p)
{
	if(kernel_is_knltask())
		print_char_p_screen(chr, p);
	else
	{
		int32 tid = kernel_get_current_tid();
		struct Task * task = get_task_info_ptr(tid);
		if(task != NULL)
			if(task->stderr == NULL)
				print_char_p_screen(chr, p);
			else
				fappend(task->stderr, &chr, 1);
	}
}

/**
	@Function:		print_err_char
	@Access:		Public
	@Description:
		打印一个字符到标准错误。
		内核任务的标准错误永远是屏幕。
		用户任务的标准错误则根据struct Task结构体的stderr字段指定。
	@Parameters:
		chr, int8, IN
			字符。
	@Return:	
*/
void
print_err_char(IN int8 chr)
{
	print_err_char_p(chr, char_color);
}

/**
	@Function:		print_err_str
	@Access:		Public
	@Description:
		打印字符串到标准错误。
		内核任务的标准错误永远是屏幕。
		用户任务的标准错误则根据struct Task结构体的stderr字段指定。
	@Parameters:
		str, const int8 *, IN
			字符串。
	@Return:
*/
void
print_err_str(IN const int8 * str)
{
	if(kernel_is_knltask())
		while(*str != '\0')
			print_err_char(*(str++));
	else
	{
		int32 tid = kernel_get_current_tid();
		struct Task * task = get_task_info_ptr(tid);
		if(task != NULL)
			if(task->stderr == NULL)
				while(*str != '\0')
					print_char(*(str++));
			else
				fappend(task->stderr, str, strlen(str));
	}
}

/**
	@Function:		print_err_str_p
	@Access:		Public
	@Description:
		打印字符串到标准错误，并且附带字符属性。
		内核任务的标准错误永远是屏幕。
		用户任务的标准错误则根据struct Task结构体的stderr字段指定。
	@Parameters:
		str, const int8 *, IN
			字符串。
		p, uint8, IN
			字符颜色属性。
	@Return:
*/
void
print_err_str_p(IN const int8 * str,
				IN uint8 p)
{
	if(kernel_is_knltask())
		while(*str != '\0')
			print_err_char_p(*(str++), p);
	else
	{
		int32 tid = kernel_get_current_tid();
		struct Task * task = get_task_info_ptr(tid);
		if(task != NULL)
			if(task->stderr == NULL)
				while(*str != '\0')
					print_char_p(*(str++), p);
			else
				fappend(task->stderr, str, strlen(str));
	}
}

/**
	@Function:		clear_screen
	@Access:		Public
	@Description:
		清除屏幕。
	@Parameters:
	@Return:		
*/
void
clear_screen(void)
{
	int32 i;
	set_cursor(0, 0);
	for(i = 0; i < COLUMN * ROW; i++)
		print_char(' ');
	set_cursor(0, 0);
}

/**
	@Function:		init_screen
	@Access:		Public
	@Description:
		初始化屏幕功能。
	@Parameters:
	@Return:		
*/
void
init_screen(void)
{
	if(!vesa_is_valid())
		clear_screen();
	else
	{
		uint32 ui;
		char_color = CC_GRAYWHITE;
		struct die_info info;
		screen_width = vesa_get_width();
		screen_height = vesa_get_height();
		if(!new_empty_image0(&screen_buffer, screen_width, screen_height))
		{
			fill_info(info, DC_INIT_SCREEN, DI_INIT_SCREEN);
			die(&info);
		}

		if(!new_empty_image0(&bg_image, screen_width, screen_height))
		{
			fill_info(info, DC_INIT_SCREEN, DI_INIT_SCREEN);
			die(&info);
		}
		fill_image_by_byte(&bg_image, 0xff);
		
		if(!new_empty_image0(&pointer_image, SYSTEM_POINTER_WIDTH, SYSTEM_POINTER_HEIGHT))
		{
			fill_info(info, DC_INIT_SCREEN, DI_INIT_SCREEN);
			die(&info);
		}
		fill_image_by_byte(&pointer_image, 0x00);

		int32 screen_char_buffer_len = ROW * COLUMN * 2;
		screen_char_buffer = alloc_memory(screen_char_buffer_len);
		if(screen_char_buffer == NULL)
		{
			fill_info(info, DC_INIT_SCREEN, DI_INIT_SCREEN);
			die(&info);
		}
		for(ui = 0; ui < screen_char_buffer_len; ui += 2)
		{
			screen_char_buffer[ui] = ' ';
			screen_char_buffer[ui + 1] = CC_BLACK;
		}

		ASCCHAR bgfile[1024];
		if(screen_width == 640 && screen_height == 480)
			config_gui_get_string("Background640x480", bgfile, sizeof(bgfile));
		else if(screen_width == 800 && screen_height == 600)
			config_gui_get_string("Background800x600", bgfile, sizeof(bgfile));
		else if(screen_width == 1024 && screen_height == 768)
			config_gui_get_string("Background1024x768", bgfile, sizeof(bgfile));
		else if(screen_width == 1280 && screen_height == 1024)
			config_gui_get_string("Background1280x1024", bgfile, sizeof(bgfile));
		IMGLBMPPtr bgbmpobj = imgl_bmp_create(bgfile);
		if(bgbmpobj != NULL)
		{
			int32 bgbmp_width, bgbmp_height;
			int32 x, y;
			destroy_common_image(&bg_image);
			new_empty_image0(&bg_image, screen_width, screen_height);
			bgbmp_width = imgl_bmp_get_width(bgbmpobj);
			bgbmp_height = imgl_bmp_get_height(bgbmpobj);
			for(x = 0; x < (int32)screen_width && x < bgbmp_width; x++)
				for(y = 0; y < (int32)screen_height && y < bgbmp_height; y++)
				{
					uint32 pixel = imgl_bmp_get_color(bgbmpobj, x, y);
					set_pixel_common_image(&bg_image, x, y, pixel);
				}
			imgl_bmp_destroy(bgbmpobj);
		}

		FILE * fptr = NULL;
		fptr = fopen(SYSTEM_POINTER, FILE_MODE_READ);
		if(fptr != NULL)
		{
			destroy_common_image(&pointer_image);
			uint8 * pointer_image_data = alloc_memory(flen(fptr));
			fread(fptr, pointer_image_data, flen(fptr));
			fclose(fptr);
			new_common_image(&pointer_image, pointer_image_data);
			free_memory(pointer_image_data);
		}

		//Window Mode
		console_screen_width = COLUMN * ENFONT_WIDTH;
		console_screen_height = ROW * (ENFONT_HEIGHT + CURSOR_HEIGHT);
		if(!new_empty_image0(&console_screen_buffer, console_screen_width, console_screen_height))
		{
			fill_info(info, DC_INIT_SCREEN, DI_INIT_SCREEN);
			die(&info);
		}
		init_window_resources();
		for(ui = 0; ui < MAX_WINDOW_COUNT; ui++)
		{
			windows[ui] = alloc_memory(sizeof(struct Window));
			if(windows[ui] == NULL)
			{
				fill_info(info, DC_INIT_SCREEN, DI_INIT_SCREEN);
				die(&info);
			}
			windows[ui]->id = 0;
		}

		init_sfunc_window();
		init_console_window();
		init_clock_window();
	}
}

/**
	@Function:		print_date
	@Access:		Public
	@Description:
		打印日期。
	@Parameters:
		date, struct CMOSDateTime *, IN
			日期。
	@Return:	
*/
void
print_date(IN struct CMOSDateTime * date)
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

/**
	@Function:		print_time
	@Access:		Public
	@Description:
		打印时间。
	@Parameters:
		time, struct CMOSDateTime *, IN
			时间。
	@Return:	
*/
void
print_time(IN struct CMOSDateTime * time)
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

/**
	@Function:		print_datetime
	@Access:		Public
	@Description:
		打印日期和时间。
	@Parameters:
		dt, struct CMOSDateTime *, IN
			时间。
	@Return:	
*/
void
print_datetime(IN struct CMOSDateTime * dt)
{
	print_date(dt);
	print_str(" ");
	print_time(dt);
}

/**
	@Function:		set_target_screen
	@Access:		Public
	@Description:
		设置目标屏幕。
	@Parameters:
		ts, struct CommonImage *, IN
			目标屏幕。
	@Return:	
*/
void
set_target_screen(IN struct CommonImage * ts)
{
	target_screen = ts;
}

/*
	============================== 窗体 ==============================
*/

/**
	@Function:		property_to_real_color
	@Access:		Private
	@Description:
		把属性颜色转换为真彩色。
	@Parameters:
		p, uint8, IN
			属性颜色。
	@Return:
		uint32
			真彩色。		
*/
static
uint32
property_to_real_color(IN uint8 p)
{
	switch(p)
	{
		case CC_BLACK:
			return VESA_RGB(0x00, 0x00, 0x00);
		case CC_BLUE:
			return VESA_RGB(0x00, 0x00, 0xFF);
		case CC_GREEN:
			return VESA_RGB(0x00, 0xFF, 0x00);
		case CC_DARKBLUE:
			return VESA_RGB(0x00, 0x00, 0x99);
		case CC_RED:
			return VESA_RGB(0xFF, 0x00, 0x00);
		case CC_MAGENTA:
			return VESA_RGB(0xFF, 0x00, 0xFF);
		case CC_BROWN:
			return VESA_RGB(0x96, 0x4B, 0x00);
		case CC_GRAYWHITE:
			return VESA_RGB(0xAA, 0xAA, 0xAA);
		case CC_LIGHTGRAY:
			return VESA_RGB(0xEE, 0xEE, 0xEE);
		case CC_LIGHTBLUE:
			return VESA_RGB(0x00, 0x80, 0xFF);
		case CC_LIGHTGREEN:
			return VESA_RGB(0xE0, 0xFF, 0xFF);
		case CC_LIGHTCYAN:
			return VESA_RGB(0x00, 0xFF, 0xFF);
		case CC_LIGHTRED:
			return VESA_RGB(0xFE, 0x00, 0x7F);
		case CC_LIGHTMAGENTA:
			return VESA_RGB(0xFF, 0xC0, 0xCB);
		case CC_YELLOW:
			return VESA_RGB(0xFF, 0xFF, 0x00);
		case CC_WHITE:
			return VESA_RGB(0xFF, 0xFF, 0xFF);
	}
}

/**
	@Function:		new_window
	@Access:		Private
	@Description:
		新建窗体。
	@Parameters:
	@Return:
		struct Window * 
			指向窗体结构体的指针。		
*/
static 
struct Window * 
new_window(void)
{
	uint32 ui;
	for(ui = 0; ui < MAX_WINDOW_COUNT; ui++)
		if(windows[ui]->id == 0)
		{
			windows[ui]->id = windows[ui];
			window_count++;
			return windows[ui];
		}
	return NULL;
}

/**
	@Function:		free_window
	@Access:		Private
	@Description:
		释放窗体。
	@Parameters:
		window, struct Window *, OUT
			指向窗体结构体的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
free_window(OUT struct Window * window)
{
	if(window == NULL)
		return FALSE;
	uint32 ui, ui1;
	for(ui = 0; ui < MAX_WINDOW_COUNT; ui++)
		if(windows[ui] == window)
		{
			for(ui1 = ui; ui1 < MAX_WINDOW_COUNT - 1; ui1++)
				windows[ui1] = windows[ui1 + 1];
			windows[MAX_WINDOW_COUNT - 1] = window;
			window->id = NULL;
			window_count--;
			return TRUE;
		}
	return FALSE;
}

/**
	@Function:		_switch_window
	@Access:		Private
	@Description:
		切换窗体。
		直到切换到一个非隐藏，非关闭的窗体。
	@Parameters:
		top, WindowPtr, IN
			当前最顶层的窗体的实例。
	@Return:
*/
static
void
_switch_window(WindowPtr top)
{
	if(top == NULL || window_count < 2)
		return;
	uint32 ui;
	WindowPtr temp = windows[0];
	for(ui = 0; ui < window_count - 1; ui++)
		windows[ui] = windows[ui + 1];
	windows[window_count - 1] = temp;
	if(windows[0] == top)
		return;
	if(	windows[0]->state == WINDOW_STATE_HIDDEN
		|| windows[0]->state == WINDOW_STATE_CLOSED)
		_switch_window(top);
}

/**
	@Function:		switch_window
	@Access:		Public
	@Description:
		切换窗体。
	@Parameters:
	@Return:
*/
void
switch_window(void)
{
	_switch_window(windows[0]);
}

/**
	@Function:		create_window
	@Access:		Public
	@Description:
		创建窗体。
	@Parameters:
		width, uint32, IN
			宽度。
		height, uint32, IN
			高度。
		bgcolor, uint32, IN
			窗体背景颜色。
		title, int8 *, IN
			标题。
		event, WindowEvent, IN
			窗体事件处理函数。
	@Return:
		struct Window *
			指向窗体结构体。如果创建失败则返回 NULL。	
*/
struct Window *
create_window(	IN uint32		width,
				IN uint32		height,
				IN uint32		bgcolor,
				IN uint32		style,
				IN int8 *		title,
				IN WindowEvent	event)
{
	struct Window * window = new_window();
	window->x = 0;
	window->y = 0;
	window->width = width;
	window->height = height;
	window->bgcolor = bgcolor;
	window->state = WINDOW_STATE_HIDDEN;
	window->over_close_button = 0;
	window->over_hidden_button = 0;
	window->has_close_button = style & WINDOW_STYLE_CLOSE_BUTTON;
	window->has_hidden_button = style & WINDOW_STYLE_HIDDEN_BUTTON;
	strcpy(window->title, title);
	window->event = event;
	if(!new_empty_image0(&window->workspace, width, height))
	{
		free_window(window);
		return NULL;
	}	
	if(!new_empty_image0(&window->image, width, TITLE_BAR_HEIGHT + height))
	{
		destroy_common_image(&window->workspace);
		free_window(window);
		return NULL;
	}
	window->key_count = 0;
	return window;
}

/**
	@Function:		destroy_window
	@Access:		Public
	@Description:
		销毁窗体。
	@Parameters:
		window, struct Window *, IN
			指向窗体结构体的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
destroy_window(IN struct Window * window)
{
	if(window == NULL)
		return FALSE;
	free_window(window);
	disable_memory_lock();
	destroy_common_image(&window->workspace);
	destroy_common_image(&window->image);
	enable_memory_lock();	
	return TRUE;
}

/**
	@Function:		get_top_window
	@Access:		Public
	@Description:
		获取顶层窗体的结构体的指针。
	@Parameters:
	@Return:
		struct Window *
			顶层窗体的结构体的指针。		
*/
struct Window *
get_top_window(void)
{
	if(window_count == 0)
		return NULL;
	else
		return windows[0];
}

/**
	@Function:		flush_screen
	@Access:		Public
	@Description:
		刷新屏幕。
	@Parameters:
	@Return:
*/
void
flush_screen(void)
{
	uint32 x, y;
	uint32 ui;
	uint32 * screen_buffer_data_ptr = screen_buffer.data;
	uint32 * console_screen_buffer_data_ptr = console_screen_buffer.data;

	if(target_screen != NULL)
		//rect_common_image(target_screen, 0, 0, screen_width, screen_height, 0xff000000);
		draw_common_image(target_screen, &bg_image, 0, 0, bg_image.width, bg_image.height);
	else
		//rect_common_image(&screen_buffer, 0, 0, screen_width, screen_height, 0xffaaaaaa);
		draw_common_image(&screen_buffer, &bg_image, 0, 0, bg_image.width, bg_image.height);

	//绘制控制台
	if(flush_cbuffer)
		for(y = 0; y < ROW; y++)
			for(x = 0; x < COLUMN; x++)
			{
				uint8 * offset = screen_char_buffer + (y * COLUMN + x) * 2;
				int8 chr = *offset;
				uint8 p = *(offset + 1);
				uint32 color = property_to_real_color(p & 0x0F);
				uint32 bg_color = property_to_real_color((p >> 4) & 0x0F);
				rect_common_image(	&console_screen_buffer, 
									x * ENFONT_WIDTH, 
									y * (ENFONT_HEIGHT + CURSOR_HEIGHT), 
									ENFONT_WIDTH, 
									ENFONT_HEIGHT + CURSOR_HEIGHT, 
									bg_color);
				uint8 * font = get_enfont(chr);
				if(font != NULL)
				{
					uint32 font_x, font_y;
					for(font_y = 0; font_y < ENFONT_HEIGHT; font_y++)
					{
						uint8 row = font[font_y];
						for(font_x = 0; font_x < ENFONT_WIDTH; font_x++)
							if(((row >> font_x) & 0x01))
							{
								uint32 index = 	y * console_screen_width * (ENFONT_HEIGHT + CURSOR_HEIGHT) 
												+ font_y * console_screen_width
												+ x * ENFONT_WIDTH 
												+ font_x;
								console_screen_buffer_data_ptr[index] = color;
							}
					}
				}
			}
	uint32 cursor_real_color = property_to_real_color(cursor_color);
	uint32 c_x = cursor_x, c_y = cursor_y;
	for(x = 0; x < ENFONT_WIDTH; x++)
		for(y = 0; y < CURSOR_HEIGHT; y++)
		{
			uint32 index = 	c_y * (ENFONT_HEIGHT + CURSOR_HEIGHT) * console_screen_width
							+ ENFONT_HEIGHT * console_screen_width
							+ y * console_screen_width 
							+ (uint32)c_x * ENFONT_WIDTH
							+ x;
			console_screen_buffer_data_ptr[index] = cursor_real_color;
		}

	//绘制窗体
	int mouse_x, mouse_y;
	get_mouse_position(&mouse_x, &mouse_y);
	if(window_count != 0)
	{
		struct Window * top_window = windows[0];
		if(point_in_rect(	mouse_x, mouse_y, 
							top_window->x + (top_window->width - CLOSE_BUTTON_WIDTH),
							top_window->y, 
							CLOSE_BUTTON_WIDTH, 
							CLOSE_BUTTON_HEIGHT))
			top_window->over_close_button = 1;
		else
			top_window->over_close_button = 0;
		if(point_in_rect(	mouse_x, mouse_y, 
							top_window->x + (top_window->width - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH),
							top_window->y, 
							HIDDEN_BUTTON_WIDTH, 
							HIDDEN_BUTTON_HEIGHT))
			top_window->over_hidden_button = 1;
		else
			top_window->over_hidden_button = 0;
	}
	mouse_left_button_down = is_mouse_left_button_down();
	if(mouse_left_button_down)
	{
		if(window_count != 0)
		{
			struct Window * top_window = windows[0];
			if(!move_window)
				if(	top_window->has_close_button
					&& point_in_rect(	mouse_x, 
										mouse_y, 
										top_window->x + top_window->width - CLOSE_BUTTON_WIDTH, 
										top_window->y, 
										CLOSE_BUTTON_WIDTH, 
										CLOSE_BUTTON_HEIGHT))
				{
					destroy_window(top_window);
					return;
				}
				else if(top_window->has_hidden_button
						&& point_in_rect(	mouse_x, 
											mouse_y, 
											top_window->x + top_window->width - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH, 
											top_window->y, 
											CLOSE_BUTTON_WIDTH, 
											CLOSE_BUTTON_HEIGHT))
				{
					top_window->state = WINDOW_STATE_HIDDEN;
					for(ui = 0; ui < window_count - 1; ui++)
						windows[ui] = windows[ui + 1];
					windows[window_count - 1] = top_window;
				}
				else if(point_in_rect(	mouse_x, 
										mouse_y, 
										top_window->x, 
										top_window->y, 
										top_window->width, 
										TITLE_BAR_HEIGHT))
				{
					old_mouse_x = mouse_x;
					old_mouse_y = mouse_y;
					move_window = TRUE;
				}				
				else
					for(ui = 1; ui < window_count; ui++)
					{
						struct Window * window = windows[ui];
						if(point_in_rect(	mouse_x, 
											mouse_y, 
											window->x, 
											window->y, 
											window->width, 
											TITLE_BAR_HEIGHT))
						{
							windows[ui] = windows[0];
							windows[0] = window;
						}
					}
		}
	}
	else if(move_window)
		move_window = FALSE;
	if(move_window)
	{
		int offset_x = mouse_x - old_mouse_x;
		int offset_y = mouse_y - old_mouse_y;
		struct Window * top_window = windows[0];
		top_window->x += offset_x;
		top_window->y += offset_y;
		old_mouse_x = mouse_x;
		old_mouse_y = mouse_y;
	}
	int32 i;
	for(i = window_count - 1; i >= 0; i--)
	{
		struct Window * window = windows[i];
		if(window->state == WINDOW_STATE_HIDDEN)
			continue;
		if(window->state == WINDOW_STATE_CLOSED)
			continue;
		if(render_window(window, &window->image, i == 0 ? TRUE : FALSE))
			draw_common_image(	&screen_buffer, 
								&window->image, 
								window->x, 
								window->y, 
								window->image.width, 
								window->image.height);
	}

	draw_common_image_t(&screen_buffer, 
						&pointer_image, 
						mouse_x, 
						mouse_y, 
						pointer_image.width, 
						pointer_image.height, 
						TCOLOR);

	if(target_screen == NULL)	
		vesa_draw_image(0, 
						0, 
						screen_width, 
						screen_height, 
						screen_buffer.data);
	else
		draw_common_image(	target_screen, 
							&screen_buffer, 
							0, 
							0, 
							screen_width, 
							screen_height);
}
