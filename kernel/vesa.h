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
VesaInit(void);

extern
uint8 *
VesaGetVideoBufferAddress(void);

extern
BOOL
VesaIsEnabled(void);

extern
uint32
VesaGetWidth(void);

extern
uint32
VesaGetHeight(void);

extern
void
VesaSetPixel(	IN uint32 x,
				IN uint32 y,
				IN uint32 color);

extern
uint32
VesaGetPixel(	IN uint32 x,
				IN uint32 y);

extern
void
VesaClearVideoBuffer(IN uint32 color);

extern
void
VesaDrawHLine(	IN uint32 x,
				IN uint32 y,
				IN uint32 len,
				IN uint32 color);

extern
void
VesaDrawRect(	IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 color);

extern
void
VesaDrawImage(	IN uint32 x,
				IN uint32 y,
				IN uint32 width,
				IN uint32 height,
				IN uint32 * image);

#endif
