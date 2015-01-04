//Filename:		vesa.c
//Author:		Ihsoh
//Date:			2014-7-24
//Descriptor:	VESA

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

uint8 *
get_vesa_addr(void)
{
	return vesa_addr_ptr;
}

BOOL
vesa_is_valid(void)
{
	return use_vesa;
}

uint32
vesa_get_width(void)
{
	return vesa_width;
}

uint32
vesa_get_height(void)
{
	return vesa_height;
}

void
vesa_set_pixel(uint32 x, uint32 y, uint32 color)
{
	if(x >= vesa_width || y >= vesa_height)
		return;
	uint8 * offset = vesa_addr_ptr + (y * vesa_width + x) * 3;
	offset[0] = ((uchar *)&color)[0];
	offset[1] = ((uchar *)&color)[1];
	offset[2] = ((uchar *)&color)[2];
}

uint32
vesa_get_pixel(uint32 x, uint y)
{
	if(x >= vesa_width || y >= vesa_height)
		return 0x00000000;
	uint8 * offset = vesa_addr_ptr + (y * vesa_width + x) * 3;
	uint32 pixel = 0;
	pixel = (uint32)(offset[2]) << 16 + (uint32)(offset[1]) << 8 + offset[0];
	return pixel;
}

void
vesa_clear_screen(uint32 color)
{
	uint32 x, y;
	for(x = 0; x < vesa_width; x++)
		for(y = 0; y < vesa_height; y++)
			vesa_set_pixel(x, y, color);
}

void
vesa_draw_line(uint32 x, uint32 y, uint32 len, uint32 color)
{
	uint32 ui;
	for(ui = 0; ui < len; ui++)
		vesa_set_pixel(x + ui, y, color);
}

void
vesa_draw_rect(uint32 x, uint32 y, uint32 width, uint32 height, uint32 color)
{
	uint32 ui;
	for(ui = 0; ui < height; ui++)
		vesa_draw_line(x, y + ui, width, color);
}

void
vesa_draw_image(uint32 x, uint32 y, uint32 width, uint32 height, uint32 * image)
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
