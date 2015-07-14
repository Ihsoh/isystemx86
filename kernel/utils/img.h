/**
	@File:			img.h
	@Author:		Ihsoh
	@Date:			2015-07-15
	@Description:
*/
#ifndef	_UTILS_IMG_H_
#define	_UTILS_IMG_H_

#include "../types.h"
#include "../bmp/bmp.h"
#include "../image.h"

extern
BOOL
img_draw_bmp_to_cimage(	IN OUT IMGLBMPPtr bmpobj,
						IN int32 width,
						IN int32 height,
						IN struct CommonImage * cimage);

#endif
