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
#include "system.h"
#include "die.h"
#include "kernel.h"
#include "image.h"
#include "window.h"
#include "klib.h"
#include "cmlock.h"
#include "config.h"

#include "bmp/bmp.h"

#include "windows/clock.h"
#include "windows/console.h"
#include "windows/sfunc.h"
#include "windows/detail.h"
#include "windows/explorer.h"
#include "windows/taskbar.h"
#include "windows/start.h"
#include "windows/power.h"
#include "windows/wmgr.h"
#include "windows/run.h"
#include "windows/dbgout.h"
#include "windows/taskmgr.h"

#include "fs/ifs1/fs.h"

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
static BOOL flush_cbuffer = TRUE;

struct CommonImage * target_screen = NULL;

#define	_BGMODE_IMAGE		0x00000000
#define	_BGMODE_PURE_COLOR	0x00000001
#define	_BGMODE_GRADIENT	0x00000002

static BOOL _background_mode		= _BGMODE_IMAGE;
static uint32 _pure_color_bg		= 0xff000000;
static uint32 _gradient_bg_lcolor	= 0xffffffff;
static uint32 _gradient_bg_rcolor	= 0xff000000;

#define	TCOLOR				0xffffffff

static struct Window * windows[MAX_WINDOW_COUNT];
static uint32 window_count = 0;
static BOOL move_window = 0;
static BOOL mouse_left_button_down = 0;
static int32 old_mouse_x = 0, old_mouse_y = 0;

/**
	@Function:		ScrWriteConsoleBuffer
	@Access:		Public
	@Description:
		写控制台字符缓冲区。
	@Parameters:
		buffer, const uint8 *, IN
			要写入的字符的缓冲区。
		size, uint32, IN
			缓冲区大小。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
ScrWriteConsoleBuffer(	IN const uint8 * buffer,
						IN uint32 size)
{
	uint8 * off;
	if(VesaIsEnabled())
			off = (uint8 *)screen_char_buffer;
		else
			off = (uint8 *)VIDEO_MEM;
	if(size > ROW * COLUMN * 2)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < size; ui++)
		off[ui] = buffer[ui];
	return TRUE;
}

/**
	@Function:		ScrEnableFlushConsoleBuffer
	@Access:		Public
	@Description:
		允许刷新字符缓冲区。
	@Parameters:
	@Return:	
*/
void
ScrEnableFlushConsoleBuffer(void)
{
	flush_cbuffer = TRUE;
}

/**
	@Function:		ScrDisableFlushConsoleBuffer
	@Access:		Public
	@Description:
		禁止刷新字符缓冲区。
	@Parameters:
	@Return:	
*/
void
ScrDisableFlushConsoleBuffer(void)
{
	flush_cbuffer = FALSE;
}

/**
	@Function:		ScrSetConsoleCharColor
	@Access:		Public
	@Description:
		设置字符颜色属性。
	@Parameters:
		cc, uint8, IN
			字符颜色属性。
	@Return:	
*/
void
ScrSetConsoleCharColor(IN uint8 cc)
{
	char_color = cc;
}

/**
	@Function:		ScrGetConsoleCharColor
	@Access:		Public
	@Description:
		获取字符颜色属性。
	@Parameters:
	@Return:
		uint8
			字符颜色属性。		
*/
uint8
ScrGetConsoleCharColor(void)
{
	return char_color;
}

/**
	@Function:		ScrSetConsoleCursorColor
	@Access:		Public
	@Description:
		设置光标颜色属性。
	@Parameters:
		cc, uint8, IN
			光标颜色属性。
	@Return:		
*/
void
ScrSetConsoleCursorColor(IN uint8 cc)
{
	cursor_color = cc;
}

/**
	@Function:		ScrGetConsoleCursorColor
	@Access:		Public
	@Description:
		获取光标颜色属性。
	@Parameters:
	@Return:
		uint8
			光标颜色属性。		
*/
uint8
ScrGetConsoleCursorColor(void)
{
	return cursor_color;
}

/**
	@Function:		ScrLockConsoleCursor
	@Access:		Public
	@Description:
		锁定光标。
	@Parameters:
	@Return:	
*/
void
ScrLockConsoleCursor(void)
{
	lock_csr = 1;
}

/**
	@Function:		ScrUnlockConsoleCursor
	@Access:		Public
	@Description:
		解锁光标。
	@Parameters:
	@Return:	
*/
void
ScrUnlockConsoleCursor(void)
{
	lock_csr = 0;
}

/**
	@Function:		ScrSetConsoleCursorPosition
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
ScrSetConsoleCursorPosition(IN uint16 x,
							IN uint16 y)
{
	cursor_x = x;
	cursor_y = y;
}

/**
	@Function:		ScrSetConsoleCursor
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
ScrSetConsoleCursor(IN uint16 x,
					IN uint16 y)
{
	ScrSetConsoleCursorPosition(x, y);
	if(!lock_csr)
		if(!VesaIsEnabled())
		{
			uint16 off = y * COLUMN + x;
			uint8 offl = (uint8)off;
			uint8 offh = (uint8)(off >> 8);
			KnlOutByte(0x3d4, 0x0e);
			KnlOutByte(0x3d5, offh);
			KnlOutByte(0x3d4, 0x0f);
			KnlOutByte(0x3d5, offl);
		}
}

/**
	@Function:		ScrGetConsoleCursor
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
ScrGetConsoleCursor(OUT uint16 * x,
					OUT uint16 * y)
{
	*x = cursor_x;
	*y = cursor_y;
}

/**
	@Function:		ScrScreenUp
	@Access:		Public
	@Description:
		向上滚一行。
	@Parameters:
	@Return:	
*/
void
ScrScreenUp(void)
{
	uint8 * dst;
	uint8 * src;
	if(VesaIsEnabled())
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
	@Function:		ScrPrintCharToScreenWithProperty
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
ScrPrintCharToScreenWithProperty(	IN int8 chr,
									IN uint8 p)
{
	uint8 * off;
	if(chr == '\n')
		if(cursor_y == ROW - 1)
		{
			ScrScreenUp();
			ScrSetConsoleCursor(0, cursor_y);
		}
		else
			ScrSetConsoleCursor(0, cursor_y + 1);
	else if(chr == '\r')
		ScrSetConsoleCursor(0, cursor_y);
	else if(chr == '\t')
	{
		uint16 x = ((uint16)(cursor_x / 4) + 1) * 4;
		if(x < COLUMN)
			ScrSetConsoleCursor(x, cursor_y);
		else
			ScrSetConsoleCursor(COLUMN - 1, cursor_y);
	}
	else
	{	
		if(VesaIsEnabled())
			off = (uint8 *)screen_char_buffer + cursor_y * COLUMN * 2 + cursor_x * 2;
		else
			off = (uint8 *)VIDEO_MEM + cursor_y * COLUMN * 2 + cursor_x * 2;
		*off = chr;
		*(off + 1) = p;
		if(cursor_x == COLUMN - 1 && cursor_y == ROW - 1)
		{
			ScrScreenUp();
			ScrSetConsoleCursor(0, cursor_y);
		}
		else if(cursor_x == COLUMN - 1 && cursor_y != ROW - 1)
			ScrSetConsoleCursor(0, cursor_y + 1);
		else
			ScrSetConsoleCursor(cursor_x + 1, cursor_y);
	}
}

/**
	@Function:		ScrPrintCharWithProperty
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
ScrPrintCharWithProperty(	IN int8 chr,
				IN uint8 p)
{
	if(KnlIsCurrentlyKernelTask())
		ScrPrintCharToScreenWithProperty(chr, p);
	else
	{
		int32 tid = KnlGetCurrentTaskId();
		struct Task * task = TskmgrGetTaskInfoPtr(tid);
		if(task != NULL)
			if(task->stdout == NULL)
				ScrPrintCharToScreenWithProperty(chr, p);
			else
				Ifs1AppendFile(task->stdout, &chr, 1);
	}
}

/**
	@Function:		ScrPrintChar
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
ScrPrintChar(IN int8 chr)
{
	ScrPrintCharWithProperty(chr, char_color);
}

/**
	@Function:		ScrPrintString
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
ScrPrintString(IN const int8 * str)
{
	if(KnlIsCurrentlyKernelTask())
		while(*str != '\0')
			ScrPrintChar(*(str++));
	else
	{
		int32 tid = KnlGetCurrentTaskId();
		struct Task * task = TskmgrGetTaskInfoPtr(tid);
		if(task != NULL)
			if(task->stdout == NULL)
				while(*str != '\0')
					ScrPrintChar(*(str++));
			else
				Ifs1AppendFile(task->stdout, str, strlen(str));
	}
}

/**
	@Function:		ScrPrintStringWithProperty
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
ScrPrintStringWithProperty(	IN const int8 * str,
							IN uint8 p)
{
	if(KnlIsCurrentlyKernelTask())
		while(*str != '\0')
			ScrPrintCharWithProperty(*(str++), p);
	else
	{
		int32 tid = KnlGetCurrentTaskId();
		struct Task * task = TskmgrGetTaskInfoPtr(tid);
		if(task != NULL)
			if(task->stdout == NULL)
				while(*str != '\0')
					ScrPrintCharWithProperty(*(str++), p);
			else
				Ifs1AppendFile(task->stdout, str, strlen(str));
	}
}

/**
	@Function:		ScrPrintCharToStderrWithProperty
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
ScrPrintCharToStderrWithProperty(	IN int8 chr,
									IN uint8 p)
{
	if(KnlIsCurrentlyKernelTask())
		ScrPrintCharToScreenWithProperty(chr, p);
	else
	{
		int32 tid = KnlGetCurrentTaskId();
		struct Task * task = TskmgrGetTaskInfoPtr(tid);
		if(task != NULL)
			if(task->stderr == NULL)
				ScrPrintCharToScreenWithProperty(chr, p);
			else
				Ifs1AppendFile(task->stderr, &chr, 1);
	}
}

/**
	@Function:		ScrPrintCharToStderr
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
ScrPrintCharToStderr(IN int8 chr)
{
	ScrPrintCharToStderrWithProperty(chr, char_color);
}

/**
	@Function:		ScrPrintStringToStderr
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
ScrPrintStringToStderr(IN const int8 * str)
{
	if(KnlIsCurrentlyKernelTask())
		while(*str != '\0')
			ScrPrintCharToStderr(*(str++));
	else
	{
		int32 tid = KnlGetCurrentTaskId();
		struct Task * task = TskmgrGetTaskInfoPtr(tid);
		if(task != NULL)
			if(task->stderr == NULL)
				while(*str != '\0')
					ScrPrintChar(*(str++));
			else
				Ifs1AppendFile(task->stderr, str, strlen(str));
	}
}

/**
	@Function:		ScrPrintStringToStderrWithProperty
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
ScrPrintStringToStderrWithProperty(	IN const int8 * str,
									IN uint8 p)
{
	if(KnlIsCurrentlyKernelTask())
		while(*str != '\0')
			ScrPrintCharToStderrWithProperty(*(str++), p);
	else
	{
		int32 tid = KnlGetCurrentTaskId();
		struct Task * task = TskmgrGetTaskInfoPtr(tid);
		if(task != NULL)
			if(task->stderr == NULL)
				while(*str != '\0')
					ScrPrintCharWithProperty(*(str++), p);
			else
				Ifs1AppendFile(task->stderr, str, strlen(str));
	}
}

/**
	@Function:		ScrClearScreen
	@Access:		Public
	@Description:
		清除屏幕。
	@Parameters:
	@Return:		
*/
void
ScrClearScreen(void)
{
	int32 i;
	ScrSetConsoleCursor(0, 0);
	for(i = 0; i < COLUMN * ROW; i++)
		ScrPrintChar(' ');
	ScrSetConsoleCursor(0, 0);
}

/**
	@Function:		ScrInit
	@Access:		Public
	@Description:
		初始化屏幕功能。
	@Parameters:
	@Return:		
*/
void
ScrInit(void)
{
	if(!VesaIsEnabled())
		ScrClearScreen();
	else
	{
		// 获取关于背景的设置。
		uint32 uiv = 0;
		if(config_gui_get_uint("BackgroundMode", &uiv))
			_background_mode = uiv;
		uiv = 0;
		if(config_gui_get_uint("PureColorBackground", &uiv))
			_pure_color_bg = uiv;
		uiv = 0;
		if(config_gui_get_uint("GradientBackgroundLeftColor", &uiv))
			_gradient_bg_lcolor = uiv;
		uiv = 0;
		if(config_gui_get_uint("GradientBackgroundRightColor", &uiv))
			_gradient_bg_rcolor = uiv;

		uint32 ui;
		char_color = CC_GRAYWHITE;
		struct die_info info;
		screen_width = VesaGetWidth();
		screen_height = VesaGetHeight();
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
		screen_char_buffer = MemAlloc(screen_char_buffer_len);
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

		// 使用图片来设置背景。
		if(_background_mode == _BGMODE_IMAGE)
		{
			ASCCHAR bgfile[1024];
			if(screen_width == 640 && screen_height == 480)
				config_gui_get_string("Background640x480", bgfile, sizeof(bgfile));
			else if(screen_width == 800 && screen_height == 600)
				config_gui_get_string("Background800x600", bgfile, sizeof(bgfile));
			else if(screen_width == 1024 && screen_height == 768)
				config_gui_get_string("Background1024x768", bgfile, sizeof(bgfile));
			else if(screen_width == 1280 && screen_height == 1024)
				config_gui_get_string("Background1280x1024", bgfile, sizeof(bgfile));
			IMGLBMPPtr bgbmpobj = BmpCreate(bgfile);
			if(bgbmpobj != NULL)
			{
				int32 bgbmp_width, bgbmp_height;
				int32 x, y;
				destroy_common_image(&bg_image);
				new_empty_image0(&bg_image, screen_width, screen_height);
				bgbmp_width = BmpGetWidth(bgbmpobj);
				bgbmp_height = BmpGetHeight(bgbmpobj);
				for(x = 0; x < (int32)screen_width && x < bgbmp_width; x++)
					for(y = 0; y < (int32)screen_height && y < bgbmp_height; y++)
					{
						uint32 pixel = BmpGetPixel(bgbmpobj, x, y);
						set_pixel_common_image(&bg_image, x, y, pixel);
					}
				BmpDestroy(bgbmpobj);
			}
		}

		// 使用渐变色来设置背景。
		if(_background_mode == _BGMODE_GRADIENT)
			draw_gradient_common_image(	&bg_image,
										_gradient_bg_lcolor,
										_gradient_bg_rcolor);

		ASCCHAR pointer_file[1024];
		IMGLBMPPtr pointer_bmpobj = NULL;
		config_gui_get_string("Pointer", pointer_file, sizeof(pointer_file));
		pointer_bmpobj = BmpCreate(pointer_file);
		int32 pointer_width = BmpGetWidth(pointer_bmpobj);
		int32 pointer_height = BmpGetHeight(pointer_bmpobj);
		new_empty_image0(&pointer_image, pointer_width, pointer_height);
		if(pointer_bmpobj != NULL)
		{
			UtlConvertBmpToCommonImage(pointer_bmpobj, pointer_width, pointer_height, &pointer_image);
			BmpDestroy(pointer_bmpobj);
		}

		//Window Mode
		console_screen_width = COLUMN * ENFONT_WIDTH;
		console_screen_height = ROW * (ENFONT_HEIGHT + CURSOR_HEIGHT);
		if(!new_empty_image0(&console_screen_buffer, console_screen_width, console_screen_height))
		{
			fill_info(info, DC_INIT_SCREEN, DI_INIT_SCREEN);
			die(&info);
		}
		WinmgrInit();
		for(ui = 0; ui < MAX_WINDOW_COUNT; ui++)
		{
			windows[ui] = MemAlloc(sizeof(struct Window));
			if(windows[ui] == NULL)
			{
				fill_info(info, DC_INIT_SCREEN, DI_INIT_SCREEN);
				die(&info);
			}
			windows[ui]->id = 0;
		}

		WinTskbrInit();
		WinStartInit();
		WinPwrInit();
		WinWmgrInit();
		// WinSfuncInit();
		WinConsoleInit();
		// WinClockInit();
		WinDetailInit();
		WinExplInit();
		WinRunInit();
		WinDbgoutInit();
		WinTskmgrInit();
	}
}

/**
	@Function:		ScrPrintDate
	@Access:		Public
	@Description:
		打印日期。
	@Parameters:
		date, struct CMOSDateTime *, IN
			日期。
	@Return:	
*/
void
ScrPrintDate(IN struct CMOSDateTime * date)
{
	printn(date->year);
	ScrPrintString("-");
	if(date->month < 10)
		ScrPrintString("0");
	printn(date->month);
	ScrPrintString("-");
	if(date->day < 10)
		ScrPrintString("0");
	printn(date->day);
}

/**
	@Function:		ScrPrintTime
	@Access:		Public
	@Description:
		打印时间。
	@Parameters:
		time, struct CMOSDateTime *, IN
			时间。
	@Return:	
*/
void
ScrPrintTime(IN struct CMOSDateTime * time)
{
	if(time->hour < 10)
		ScrPrintString("0");
	printn(time->hour);
	ScrPrintString(":");
	if(time->minute < 10)
		ScrPrintString("0");
	printn(time->minute);
	ScrPrintString(":");
	if(time->second < 10)
		ScrPrintString("0");
	printn(time->second);
}

/**
	@Function:		ScrPrintDateTime
	@Access:		Public
	@Description:
		打印日期和时间。
	@Parameters:
		dt, struct CMOSDateTime *, IN
			时间。
	@Return:	
*/
void
ScrPrintDateTime(IN struct CMOSDateTime * dt)
{
	ScrPrintDate(dt);
	ScrPrintString(" ");
	ScrPrintTime(dt);
}

/**
	@Function:		ScrSetTargetScreen
	@Access:		Public
	@Description:
		设置目标屏幕。
	@Parameters:
		ts, struct CommonImage *, IN
			目标屏幕。
	@Return:	
*/
void
ScrSetTargetScreen(IN struct CommonImage * ts)
{
	target_screen = ts;
}

/*
	============================== 窗体 ==============================
*/

/**
	@Function:		_ScrConvertPropertyToRGB
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
_ScrConvertPropertyToRGB(IN uint8 p)
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
	@Function:		_ScrNewWindow
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
_ScrNewWindow(void)
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
	@Function:		_ScrFreeWindow
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
_ScrFreeWindow(OUT struct Window * window)
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
			window->id = 0;
			window_count--;
			return TRUE;
		}
	return FALSE;
}

/**
	@Function:		_ScrSwitchWindow
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
_ScrSwitchWindow(IN WindowPtr top)
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
		_ScrSwitchWindow(top);
}

/**
	@Function:		ScrSwitchWindow
	@Access:		Public
	@Description:
		切换窗体。
	@Parameters:
	@Return:
*/
void
ScrSwitchWindow(void)
{
	_ScrSwitchWindow(windows[0]);
}

/**
	@Function:		ScrSetTopWindow
	@Access:		Public
	@Description:
		把指定窗体置顶。
	@Parameters:
		window, WindowPtr, IN
			需要置顶的窗体。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
ScrSetTopWindow(IN WindowPtr window)
{
	if(window == NULL || window_count == 0)
		return FALSE;
	if(windows[0] == window)
		return TRUE;
	int32 pos;
	for(pos = 0; pos < window_count; pos++)
		if(windows[pos] == window)
			break;
	if(pos < window_count)
	{
		int32 i;
		for(i = pos - 1; i >= 0; i--)
			windows[i + 1] = windows[i];
		windows[0] = window;
		return TRUE;
	}
	else
		return FALSE;
}

/**
	@Function:		ScrCreateWindow
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
ScrCreateWindow(IN uint32		width,
				IN uint32		height,
				IN uint32		bgcolor,
				IN uint32		style,
				IN int8 *		title,
				IN WindowEvent	event)
{
	struct Window * window = _ScrNewWindow();
	window->uwid = -1;
	window->x = 0;
	window->y = 0;
	window->width = width;
	window->height = height;
	window->bgcolor = bgcolor;
	window->state = WINDOW_STATE_HIDDEN;
	window->over_close_button = 0;
	window->over_hidden_button = 0;
	window->style = style;
	UtlCopyString(window->title, sizeof(window->title), title);
	window->event = event;
	window->cb_key_press = NULL;
	window->locked = FALSE;
	window->old_state = WINDOW_STATE_NONE;
	window->is_top = FALSE;
	if(!new_empty_image0(&window->workspace, width, height))
	{
		_ScrFreeWindow(window);
		return NULL;
	}
	if(!(style & WINDOW_STYLE_NO_TITLE))
		if(!new_empty_image0(&window->title_bar, width, TITLE_BAR_HEIGHT))
		{
			destroy_common_image(&window->workspace);
			_ScrFreeWindow(window);
			return NULL;
		}
	window->key_count = 0;
	return window;
}

/**
	@Function:		ScrDestroyWindow
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
ScrDestroyWindow(IN struct Window * window)
{
	if(window == NULL || window->id == 0)
		return FALSE;
	_ScrFreeWindow(window);
	disable_memory_lock();
	destroy_common_image(&window->workspace);
	destroy_common_image(&window->title_bar);
	enable_memory_lock();	
	return TRUE;
}

/**
	@Function:		ScrGetTopWindow
	@Access:		Public
	@Description:
		获取顶层窗体的结构体的指针。
	@Parameters:
	@Return:
		struct Window *
			顶层窗体的结构体的指针。		
*/
struct Window *
ScrGetTopWindow(void)
{
	if(window_count == 0)
		return NULL;
	else
		return windows[0];
}

/**
	@Function:		ScrRenderTextBufferEx
	@Access:		Public
	@Description:
		渲染文本缓冲区。
	@Parameters:
		image, ImagePtr, IN OUT
			指向保存渲染结果的图片对象的指针。
		start_x, uint32, IN
			X坐标。在X坐标绘制渲染后的图片。
		start_y, uint32, IN
			Y坐标。在Y坐标绘制渲染后的图片。
		txtbuf, uint8 *, IN
			指向文本缓冲区的指针。
		row, uint32, IN
			文本缓冲区的行数。
		col, uint32, IN
			文本缓冲区的列数。
		curx, uint32, IN
			光标的X坐标。
		cury, uint32, IN
			光标的Y坐标。
	@Return:
		struct Window *
			顶层窗体的结构体的指针。		
*/
BOOL
ScrRenderTextBufferEx(	IN OUT ImagePtr image,
						IN uint32 start_x,
						IN uint32 start_y,
						IN uint8 * txtbuf,
						IN uint32 row,
						IN uint32 column,
						IN uint32 curx,
						IN uint32 cury)
{
	if(image == NULL || txtbuf == NULL)
		return FALSE;
	uint32 x, y;
	for(y = 0; y < row; y++)
		for(x = 0; x < column; x++)
		{
			uint8 * offset = txtbuf + (y * column + x) * 2;
			int8 chr = *offset;
			uint8 p = *(offset + 1);
			uint32 color = _ScrConvertPropertyToRGB(p & 0x0F);
			uint32 bg_color = _ScrConvertPropertyToRGB((p >> 4) & 0x0F);
			rect_common_image(	image, 
								start_x + x * ENFONT_WIDTH, 
								start_y + y * (ENFONT_HEIGHT + CURSOR_HEIGHT), 
								ENFONT_WIDTH, 
								ENFONT_HEIGHT + CURSOR_HEIGHT, 
								bg_color);
			uint8 * font = EnfntGetFontDataPtr();

			if(font != NULL)
				text_common_image(	image,
									start_x + x * ENFONT_WIDTH,
									start_y + y * (ENFONT_HEIGHT + CURSOR_HEIGHT),
									font,
									&chr,
									1,
									color);
		}
	uint32 cursor_real_color = _ScrConvertPropertyToRGB(cursor_color);
	uint32 c_x = curx, c_y = cury;
	uint32 s_w = image->width;
	for(x = 0; x < ENFONT_WIDTH; x++)
		for(y = 0; y < CURSOR_HEIGHT; y++)
		{
			uint32 index = 	c_y * (ENFONT_HEIGHT + CURSOR_HEIGHT) * s_w
							+ ENFONT_HEIGHT * s_w
							+ (start_y + y) * s_w
							+ (uint32)c_x * ENFONT_WIDTH
							+ (start_x + x);
			((uint32 *)image->data)[index] = cursor_real_color;
		}
	return TRUE;
}

/**
	@Function:		ScrRenderTextBuffer
	@Access:		Public
	@Description:
		渲染文本缓冲区。
	@Parameters:
		image, ImagePtr, IN OUT
			指向保存渲染结果的图片对象的指针。
		txtbuf, uint8 *, IN
			指向文本缓冲区的指针。
		row, uint32, IN
			文本缓冲区的行数。
		col, uint32, IN
			文本缓冲区的列数。
		curx, uint32, IN
			光标的X坐标。
		cury, uint32, IN
			光标的Y坐标。
	@Return:
		struct Window *
			顶层窗体的结构体的指针。		
*/
BOOL
ScrRenderTextBuffer(IN OUT ImagePtr image,
					IN uint8 * txtbuf,
					IN uint32 row,
					IN uint32 column,
					IN uint32 curx,
					IN uint32 cury)
{
	return ScrRenderTextBufferEx(image, 0, 0, txtbuf, row, column, curx, cury);
}

/**
	@Function:		ScrFlushScreen
	@Access:		Public
	@Description:
		刷新屏幕。
	@Parameters:
	@Return:
*/
void
ScrFlushScreen(void)
{
	uint32 x, y;
	uint32 ui;
	uint32 * screen_buffer_data_ptr = screen_buffer.data;
	uint32 * console_screen_buffer_data_ptr = console_screen_buffer.data;

	if(_background_mode == _BGMODE_PURE_COLOR)
		if(target_screen != NULL)
			fill_image_by_uint32(target_screen, _pure_color_bg);
		else
			fill_image_by_uint32(&screen_buffer, _pure_color_bg);
	else if(_background_mode == _BGMODE_IMAGE
			|| _background_mode == _BGMODE_GRADIENT)
		if(target_screen != NULL)
			draw_common_image(target_screen, &bg_image, 0, 0, bg_image.width, bg_image.height);
		else
			draw_common_image(&screen_buffer, &bg_image, 0, 0, bg_image.width, bg_image.height);

	//绘制控制台
	if(flush_cbuffer)
	{
		for(y = 0; y < ROW; y++)
			for(x = 0; x < COLUMN; x++)
			{
				uint8 * offset = screen_char_buffer + (y * COLUMN + x) * 2;
				int8 chr = *offset;
				uint8 p = *(offset + 1);
				uint32 color = _ScrConvertPropertyToRGB(p & 0x0F);
				uint32 bg_color = _ScrConvertPropertyToRGB((p >> 4) & 0x0F);
				rect_common_image(	&console_screen_buffer, 
									x * ENFONT_WIDTH, 
									y * (ENFONT_HEIGHT + CURSOR_HEIGHT), 
									ENFONT_WIDTH, 
									ENFONT_HEIGHT + CURSOR_HEIGHT, 
									bg_color);
				uint8 * font = EnfntGetFontDataPtr();

				if(font != NULL)
					text_common_image(	&console_screen_buffer,
										x * ENFONT_WIDTH,
										y * (ENFONT_HEIGHT + CURSOR_HEIGHT),
										font,
										&chr,
										1,
										color);
			}
		uint32 cursor_real_color = _ScrConvertPropertyToRGB(cursor_color);
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
	}

	//绘制窗体
	int32 mouse_x, mouse_y;
	KnlGetMousePosition(&mouse_x, &mouse_y);
	if(window_count != 0)
	{
		struct Window * top_window = windows[0];
		uint32 wstyle = top_window->style;
		BOOL has_title_bar = !(wstyle & WINDOW_STYLE_NO_TITLE);
		if(has_title_bar)
		{
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
	}
	mouse_left_button_down = KnlIsMouseLeftButtonDown();
	if(mouse_left_button_down)
	{
		if(window_count != 0)
		{
			struct Window * top_window = windows[0];
			uint32 wstyle = top_window->style;
			BOOL has_title_bar = !(wstyle & WINDOW_STYLE_NO_TITLE);
			if(!move_window)
			{
				if(has_title_bar)
				{
					if(	wstyle & WINDOW_STYLE_CLOSE
						&& point_in_rect(	mouse_x, 
											mouse_y, 
											top_window->x + top_window->width - CLOSE_BUTTON_WIDTH, 
											top_window->y, 
											CLOSE_BUTTON_WIDTH, 
											CLOSE_BUTTON_HEIGHT))
					{
						WinmgrDispatchEvent(	top_window,
												WINDOW_EVENT_WILL_CLOSE,
												NULL);
						return;
					}
					else if(wstyle & WINDOW_STYLE_MINIMIZE
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
				}
				
				// 切换窗体到最顶层。
				for(ui = 0; ui < window_count; ui++)
				{
					struct Window * window = windows[ui];
					uint32 wstyle = window->style;
					BOOL has_title_bar = !(wstyle & WINDOW_STYLE_NO_TITLE);
					if(	window->state == WINDOW_STATE_HIDDEN ||
						window->state == WINDOW_STATE_CLOSED)
						continue;
					if(wstyle & WINDOW_STYLE_NO_TITLE)
					{
						if(point_in_rect(	mouse_x, 
											mouse_y, 
											window->x, 
											window->y, 
											window->width, 
											window->height))
						{
							windows[ui] = windows[0];
							windows[0] = window;
							break;
						}
					}
					else
						if(point_in_rect(	mouse_x, 
											mouse_y, 
											window->x, 
											window->y, 
											window->width, 
											window->height + TITLE_BAR_HEIGHT))
						{
							windows[ui] = windows[0];
							windows[0] = window;
							break;
						}
				}
			}
		}
	}
	else if(move_window)
		move_window = FALSE;
	if(move_window)
	{
		int32 offset_x = mouse_x - old_mouse_x;
		int32 offset_y = mouse_y - old_mouse_y;
		struct Window * top_window = windows[0];
		top_window->x += offset_x;
		top_window->y += offset_y;
		old_mouse_x = mouse_x;
		old_mouse_y = mouse_y;
	}
	int32 i;
	for(i = window_count - 1; i >= 0; i--)
	{
		BOOL top = i == 0;
		uint32 border_color = WINDOW_BORDER_COLOR_NT;
		if(top)
			border_color = WINDOW_BORDER_COLOR;
		struct Window * window = windows[i];
		uint32 wstyle = window->style;
		BOOL has_title_bar = !(wstyle & WINDOW_STYLE_NO_TITLE);
		if(has_title_bar)
		{
			if(WinmgrRenderWindow(window, top))
			{
				draw_common_image(	&screen_buffer,
									&window->title_bar,
									window->x,
									window->y,
									window->title_bar.width,
									window->title_bar.height);
				draw_common_image(	&screen_buffer,
									&window->workspace,
									window->x,
									window->y + TITLE_BAR_HEIGHT,
									window->workspace.width,
									window->workspace.height);
				if(!(wstyle & WINDOW_STYLE_NO_BORDER))
				{
					hline_common_image( &screen_buffer,
										window->x - 1,
										window->y - 1,
										window->width + 2,
										border_color);
					hline_common_image(	&screen_buffer,
										window->x - 1,
										window->y + TITLE_BAR_HEIGHT + window->height,
										window->width + 2,
										border_color);
					vline_common_image(	&screen_buffer,
										window->x - 1,
										window->y,
										TITLE_BAR_HEIGHT + window->height,
										border_color);
					vline_common_image(	&screen_buffer,
										window->x + window->width,
										window->y,
										TITLE_BAR_HEIGHT + window->height,
										border_color);
				}
			}
		}
		else
			if(WinmgrRenderWindow(window, top))
			{
				draw_common_image(	&screen_buffer,
									&window->workspace,
									window->x,
									window->y,
									window->workspace.width,
									window->workspace.height);
				if(!(wstyle & WINDOW_STYLE_NO_BORDER))
				{
					hline_common_image( &screen_buffer,
										window->x - 1,
										window->y - 1,
										window->width + 2,
										border_color);
					hline_common_image(	&screen_buffer,
										window->x - 1,
										window->y + window->height,
										window->width + 2,
										border_color);
					vline_common_image(	&screen_buffer,
										window->x - 1,
										window->y,
										window->height,
										border_color);
					vline_common_image(	&screen_buffer,
										window->x + window->width,
										window->y,
										window->height,
										border_color);
				}
			}
	}

	draw_common_image_t(&screen_buffer, 
						&pointer_image, 
						mouse_x, 
						mouse_y, 
						pointer_image.width, 
						pointer_image.height, 
						TCOLOR);

	if(target_screen == NULL)	
		VesaDrawImage(0, 
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

/**
	@Function:		ScrGetWindows
	@Access:		Public
	@Description:
		获取指向所有窗体数组的指针。
	@Parameters:
		count, uint32 *, OUT
			窗体的数量。
	@Return:
		WindowPtr *
			指向所有窗体数组的指针。
*/
WindowPtr *
ScrGetWindows(OUT uint32 * count)
{
	*count = window_count;
	return windows;
}
