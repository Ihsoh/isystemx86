#ifndef	_BMP_BMP_H_
#define	_BMP_BMP_H_

#include "../types.h"
#include "../fs/ifs1/fs.h"
#include <dslib/list.h>

#define	BI_RGB			0
#define	BI_BITFIELDS	3

typedef	struct
{
	FileObject *	imgfile_ptr;
	uint8 *			data_ptr;
	uint8 *			pixel_array_ptr;
	int32			width, height, color_depth, compression, palette_size;
	int32			red_mask, green_mask, blue_mask;
	int32			red_shift, green_shift, blue_shift;
	DSLListPtr		palette_r;
	DSLListPtr		palette_g;
	DSLListPtr		palette_b;
	int32			row_size;
} IMGLBMP, * IMGLBMPPtr;

IMGLBMPPtr
BmpCreate(IN CASCTEXT file);

BOOL
BmpDestroy(IN IMGLBMPPtr bmpobj);

int32
BmpGetWidth(IN IMGLBMPPtr bmpobj);

int32
BmpGetHeight(IN IMGLBMPPtr bmpobj);

uint32
BmpGetPixel(IN IMGLBMPPtr bmpobj,
			IN int32 x, 
			IN int32 y);

#endif
