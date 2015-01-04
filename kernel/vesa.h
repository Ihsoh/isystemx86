//Filename:		vesa.h
//Author:		Ihsoh
//Date:			2014-7-24
//Descriptor:	VESA

#ifndef	_VESA_H_
#define	_VESA_H_

#include "types.h"

#define	VESA_RGB(r, g, b)	(((uint)(r) << 16) + ((uint)(g) << 8) + b)

extern void init_vesa(void);
extern uint8 * get_vesa_addr(void);
extern BOOL vesa_is_valid(void);
extern uint32 vesa_get_width(void);
extern uint32 vesa_get_height(void);
extern void vesa_set_pixel(uint32 x, uint32 y, uint32 color);
extern uint32 vesa_get_pixel(uint32 x, uint32 y);
extern void vesa_clear_screen(uint32 color);
extern void vesa_draw_line(uint32 x, uint32 y, uint32 len, uint32 color);
extern void vesa_draw_rect(uint32 x, uint32 y, uint32 width, uint32 height, uint32 color);
extern void vesa_draw_image(uint32 x, uint32 y, uint32 width, uint32 height, uint32 * image);

#endif
