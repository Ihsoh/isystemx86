/**
	@File:			vesa.c
	@Author:		Ihsoh
	@Date:			2014-7-24
	@Description:
		提供 VESA 的相关功能。
*/

#include "vesa.h"
#include "memory.h"
#include "kfuncs.h"
#include "types.h"
#include <string.h>

static BOOL use_vesa = FALSE;
static uint8 vesa_info[512];
static uint32 vesa_addr = 0;
static uint8 * vesa_addr_ptr = 0;
static uint32 vesa_width = 80;
static uint32 vesa_height = 25;

/**
	@Function:		init_vesa
	@Access:		Public
	@Description:
		初始化 VESA。
	@Parameters:
	@Return:	
*/
void
init_vesa(void)
{
	uint32 mode;
	uint8 * ucptr = (uint8 *)get_vesa_info_addr(&mode);
	if(ucptr != NULL)
	{
		use_vesa = TRUE;
		memcpy(vesa_info, ucptr, 512);
		switch(mode)
		{
			case 0x112:
				vesa_width = 640;
				vesa_height = 480;
				break;
			case 0x115:
				vesa_width = 800;
				vesa_height = 600;
				break;
			case 0x118:
				vesa_width = 1024;
				vesa_height = 768;
				break;
			case 0x11b:
				vesa_width = 1280;
				vesa_height = 1024;
				break;
			default:
				asm volatile ("cli; hlt;");
		}
		vesa_addr = *((uint32 *)(vesa_info + 40));
		vesa_addr_ptr = (uint8 *)vesa_addr;
		alloc_memory_with_start(vesa_addr, vesa_width * vesa_height * 3);
	}
}

/**
	@Function:		get_vesa_addr
	@Access:		Public
	@Description:
		获取视频缓冲区地址。
	@Parameters:
	@Return:
		uint8 *
			视频缓冲区地址。		
*/
uint8 *
get_vesa_addr(void)
{
	return vesa_addr_ptr;
}

/**
	@Function:		vesa_is_valid
	@Access:		Public
	@Description:
		获取 VESA 是否可用。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则可用，否则不可用。	
*/
BOOL
vesa_is_valid(void)
{
	return use_vesa;
}

/**
	@Function:		vesa_get_width
	@Access:		Public
	@Description:
		获取视频缓冲区的宽度。
	@Parameters:
	@Return:
		uint32
			视频缓冲区的宽度。		
*/
uint32
vesa_get_width(void)
{
	return vesa_width;
}

/**
	@Function:		vesa_get_height
	@Access:		Public
	@Description:
		获取视频缓冲区的高度。
	@Parameters:
	@Return:
		uint32
			视频缓冲区的高度。		
*/
uint32
vesa_get_height(void)
{
	return vesa_height;
}

/**
	@Function:		vesa_set_pixel
	@Access:		Public
	@Description:
		设置像素到视频缓冲区。
	@Parameters:
		x, uint32, IN
			X 坐标。
		y, uint32, IN
			Y 坐标。
		color, uint32, IN
			像素值。
	@Return:	
*/
void
vesa_set_pixel(	IN uint32 x,
				IN uint32 y,
				IN uint32 color)
{
	if(x >= vesa_width || y >= vesa_height)
		return;
	uint8 * offset = vesa_addr_ptr + (y * vesa_width + x) * 3;
	offset[0] = ((uchar *)&color)[0];
	offset[1] = ((uchar *)&color)[1];
	offset[2] = ((uchar *)&color)[2];
}

/**
	@Function:		vesa_get_pixel
	@Access:		Public
	@Description:
		获取视频缓冲区中得指定的像素值。
	@Parameters:
		x, uint32, IN
			X 坐标。
		y, uint32, IN
			Y 坐标。
	@Return:
		uint32
			像素值。		
*/
uint32
vesa_get_pixel(	IN uint32 x,
				IN uint32 y)
{
	if(x >= vesa_width || y >= vesa_height)
		return 0x00000000;
	uint8 * offset = vesa_addr_ptr + (y * vesa_width + x) * 3;
	uint32 pixel = 0;
	pixel = (uint32)(offset[2]) << 16 + (uint32)(offset[1]) << 8 + offset[0];
	return pixel;
}

/**
	@Function:		vesa_clear_screen
	@Access:		Public
	@Description:
		清除视频缓冲区。
	@Parameters:
		color, uint32, IN
			填充的像素值。
	@Return:	
*/
void
vesa_clear_screen(IN uint32 color)
{
	uint32 x, y;
	for(x = 0; x < vesa_width; x++)
		for(y = 0; y < vesa_height; y++)
			vesa_set_pixel(x, y, color);
}

/**
	@Function:		vesa_draw_line
	@Access:		Public
	@Description:
		画一条水平线。
	@Parameters:
		x, uint32, IN
			起始 X 坐标。
		y, uint32, IN
			起始 Y 坐标。
		len, uint32, IN
			线的长度。
		color, uint32, IN
			线的像素值。
	@Return:	
*/
void
vesa_draw_line(	IN uint32 x,
				IN uint32 y,
				IN uint32 len,
				IN uint32 color)
{
	uint32 ui;
	for(ui = 0; ui < len; ui++)
		vesa_set_pixel(x + ui, y, color);
}

/**
	@Function:		vesa_draw_rect
	@Access:		Public
	@Description:
		画一个矩形。
	@Parameters:
		x, uint32, IN
			起始 X 坐标。
		y, uint32, IN
			起始 Y 坐标。
		width, uint32, IN
			矩形的宽度。
		height, uint32, IN
			矩形的高度。
		color, uint32, IN
			线的像素值。
	@Return:	
*/
void
vesa_draw_rect(	IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 color)
{
	uint32 ui;
	for(ui = 0; ui < height; ui++)
		vesa_draw_line(x, y + ui, width, color);
}

/**
	@Function:		vesa_draw_image
	@Access:		Public
	@Description:
		往视频缓冲区画一张位图。
	@Parameters:
		x, uint32, IN
			起始 X 坐标。
		y, uint32, IN
			起始 Y 坐标。
		width, uint32, IN
			图片的宽度。
		height, uint32, IN
			图片的高度。
		image, uint32 *, IN
			指向位图的像素缓冲区的指针。
	@Return:	
*/
void
vesa_draw_image(IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 * image)
{
	uint32 ui_x, ui_y;
	for(ui_y = 0; ui_y < height; ui_y++)
		for(ui_x = 0; ui_x < width; ui_x++)
		{
			if(x + ui_x >= vesa_width || y + ui_y >= vesa_height)
				continue;
			uint32 color = *(image + (ui_y * width + ui_x));
			uint8 * offset = vesa_addr_ptr + ((y + ui_y) * vesa_width + (x + ui_x)) * 3;
			offset[0] = ((uchar *)&color)[0];
			offset[1] = ((uchar *)&color)[1];
			offset[2] = ((uchar *)&color)[2];
		}
}
