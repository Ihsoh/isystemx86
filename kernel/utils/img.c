/**
	@File:			img.h
	@Author:		Ihsoh
	@Date:			2015-07-15
	@Description:
*/

#include "img.h"
#include "../types.h"
#include "../bmp/bmp.h"
#include "../image.h"

BOOL
UtlConvertBmpToCommonImage(	IN OUT IMGLBMPPtr bmpobj,
							IN int32 width,
							IN int32 height,
							IN struct CommonImage * cimage)
{
	if(bmpobj == NULL || cimage == NULL)
		return FALSE;
	int32 bmp_width, bmp_height;
	int32 cimage_width, cimage_height;
	int32 x, y;
	bmp_width = BmpGetWidth(bmpobj);
	bmp_height = BmpGetHeight(bmpobj);
	for(x = 0; x < width && x < bmp_width && x < cimage->width; x++)
		for(y = 0; y < height && y < bmp_height && y < cimage->height; y++)
		{
			uint32 pixel = BmpGetPixel(bmpobj, x, y);
			set_pixel_common_image(cimage, x, y, pixel);
		}
	return TRUE;
}
