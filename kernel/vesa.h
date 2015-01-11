/**
	@File:			vesa.h
	@Author:		Ihsoh
	@Date:			2014-7-24
	@Description:
*/

#ifndef	_VESA_H_
#define	_VESA_H_

#include "types.h"

#define	VESA_RGB(r, g, b)	(((uint)(r) << 16) + ((uint)(g) << 8) + b)

extern
void
init_vesa(void);

extern
uint8 *
get_vesa_addr(void);

extern
BOOL
vesa_is_valid(void);

extern
uint32
vesa_get_width(void);

extern
uint32
vesa_get_height(void);

extern
void
vesa_set_pixel(	IN uint32 x,
				IN uint32 y,
				IN uint32 color);

extern
uint32
vesa_get_pixel(	IN uint32 x,
				IN uint32 y);

extern
void
vesa_clear_screen(IN uint32 color);

extern
void
vesa_draw_line(	IN uint32 x,
				IN uint32 y,
				IN uint32 len,
				IN uint32 color);

extern
void
vesa_draw_rect(	IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 color);

extern
void
vesa_draw_image(IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 * image);

#endif
