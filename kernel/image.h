/**
	@File:			image.h
	@Author:		Ihsoh
	@Date:			2014-9-25
	@Description:
*/

#ifndef	_IMAGE_H_
#define	_IMAGE_H_

#include "types.h"

#define	CMMNIMAGE_IMG0_SYMBOL	"IMAGE0"

#define	CMMNIMAGE_IMG0	0

#ifndef	ENFONT_WIDTH
	#define	ENFONT_WIDTH	8
	#define	ENFONT_HEIGHT	16
#endif

#define	IMAGE_PIXEL_RGB(r, g, b) (0xff000000 + ((uint)r << 16) + ((uint)g << 8) + b)

struct CommonImage
{
	int32	type;
	uint32	width;
	uint32	height;
	uint8 *	data;
};

extern
BOOL
new_common_image(	OUT struct CommonImage * common_image,
					IN uint8 * data);

extern
BOOL
new_empty_image0(	OUT struct CommonImage * common_image, 
					IN uint32 width,
					IN uint32 height);

extern
BOOL
destroy_common_image(IN struct CommonImage * common_image);

extern
BOOL
save_common_image(	IN struct CommonImage * common_image,
					IN int8 * path);

extern
BOOL
fill_image_by_byte(	IN OUT struct CommonImage * common_image, 
					IN uint8 data);

extern
BOOL
set_pixel_common_image(	IN OUT struct CommonImage * common_image,
						IN int32 x,
						IN int32 y,
						IN uint32 pixel);

extern
uint32
get_pixel_common_image(	IN struct CommonImage * common_image,
						IN int32 x,
						IN int32 y);

extern
BOOL
draw_common_image(	IN OUT struct CommonImage * dst, 
					IN struct CommonImage * src, 
					IN int32 draw_x,
					IN int32 draw_y,
					IN int32 width,
					IN int32 height);

extern
BOOL
draw_common_image_t(IN OUT struct CommonImage * dst,
					IN struct CommonImage * src,
					IN int32 draw_x,
					IN int32 draw_y,
					IN int32 width,
					IN int32 height,
					IN uint32 tpixel);

extern
BOOL
hline_common_image(	IN OUT struct CommonImage * common_image, 
					IN int32 start_x,
					IN int32 start_y,
					IN int32 length,
					IN uint32 pixel);

extern
BOOL
vline_common_image(	IN OUT struct CommonImage * common_image,
					IN int32 start_x,
					IN int32 start_y,
					IN int32 length,
					IN uint32 pixel);

extern
BOOL
line_common_image(	OUT struct CommonImage * common_image,
					IN int32 start_x,
					IN int32 start_y,
					IN int32 end_x,
					IN int32 end_y,
					IN uint32 pixel);

extern
BOOL
rect_common_image(	OUT struct CommonImage * common_image,
					IN int32 start_x,
					IN int32 start_y,
					IN int32 width,
					IN int32 height,
					IN uint32 pixel);

extern
BOOL
text_common_image(	OUT struct CommonImage * common_image, 
					IN int32 draw_x,
					IN int32 draw_y,
					IN uint8 * enfont,
					IN int8 * text,
					IN uint32 count,
					IN uint32 color);

#endif
