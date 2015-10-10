/**
	@File:			image.c
	@Author:		Ihsoh
	@Date:			2014-9-25
	@Description:
		提供图片操作功能。
*/

#include "image.h"
#include "types.h"

#ifdef	_KERNEL_MODEL_
	#include "enfont.h"
	#include "memory.h"
	#include "fs/ifs1/fs.h"
	#include <ilib/string.h>
	#define	ALLOCM(n)	(alloc_memory((n)))
	#define	FREEM(ptr)	(free_memory((ptr)))
#else
	#include <ilib/ilib.h>
	#define	ALLOCM(n)	(allocm((n)))
	#define	FREEM(ptr)	(freem((ptr)))
#endif

#define	IMAGE_MODE_NORMAL	1
#define	IMAGE_MODE_SSE		2
#define	IMAGE_MODE_SSE_EX	3

static int32 image_mode = IMAGE_MODE_SSE_EX;

/**
	@Function:		image_fast_fill_uint8
	@Access:		Private
	@Description:
		快速填充uint8数据。
	@Parameters:
		dest, uint8 *, OUT
			填充的目标。
		data, uint8, IN
			要填充的数据。
		count, uint32, IN
			填充uint8的数量。
	@Return:
*/
static
void
image_fast_fill_uint8(	OUT uint8 * dest,
						IN uint8 data,
						IN uint32 count)
{
	uint32 uidata = ((uint32)data << 24)
					| ((uint32)data << 16)
					| ((uint32)data << 8)
					| (uint32)data;
	uint32 ui;
	uint32 * uiptr = (uint32 *)dest;
	for(ui = 0; ui < count / 4; ui++)
		*(uiptr++) = uidata;
	dest = (uint8 *)uiptr;
	for(ui = 0; ui < count % 4; ui++)
		*(dest++) = data;
}

/**
	@Function:		new_common_image
	@Access:		Public
	@Description:
		通过文件的数据新建一张图片。
	@Parameters:
		common_image, struct CommonImage *, OUT
			图片。
		data, uint8 *, IN
			文件的数据。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
new_common_image(	OUT struct CommonImage * common_image, 
					IN uint8 * data)
{
	if(common_image == NULL || data == NULL)
		return FALSE;
	if(strcmpn(data, CMMNIMAGE_IMG0_SYMBOL, 6) == 0)
	{
		common_image->type = CMMNIMAGE_IMG0;
		common_image->width = *(uint32 *)(data + 6);
		common_image->height = *(uint32 *)(data + 6 + 4);
		uint32 image_data_len = common_image->width * common_image->height * 4;
		common_image->data = ALLOCM(image_data_len);
		if(common_image->data == NULL)
			return FALSE;
		memcpy(common_image->data, data + 6 + 4 + 4, image_data_len);
		return TRUE;
	}
	return FALSE;
}

/**
	@Function:		new_empty_image0
	@Access:		Public
	@Description:
		通过宽度和高度创建一张空白的 Image0 图片。
	@Parameters:
		common_image, struct CommonImage *, OUT
			图片。
		width, uint32, IN
			需要创建的图片的宽度。
		height, uint32, IN
			需要创建的图片的高度。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
new_empty_image0(	OUT struct CommonImage * common_image, 
					IN uint32 width,
					IN uint32 height)
{
	if(common_image == NULL || width == 0 || height == 0)
		return FALSE;
	common_image->type = CMMNIMAGE_IMG0;
	common_image->width = width;
	common_image->height = height;
	uint32 image_data_len = width * height * 4;
	common_image->data = ALLOCM(image_data_len);
	if(common_image->data == NULL)
		return FALSE;
	return TRUE;
}

/**
	@Function:		destroy_common_image
	@Access:		Public
	@Description:
		销毁图片的资源。
	@Parameters:
		common_image, struct CommonImage *, IN
			图片。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
destroy_common_image(IN struct CommonImage * common_image)
{
	if(common_image != NULL && FREEM(common_image->data))
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		save_common_image
	@Access:		Public
	@Description:
		保存图片到文件。
	@Parameters:
		common_image, struct CommonImage *, IN
			图片。
		path, int8 *, IN
			要保存的路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。			
*/
BOOL
save_common_image(	IN struct CommonImage * common_image,
					IN int8 * path)
{
	if(common_image == NULL || path == NULL)
		return FALSE;
	#ifdef	_KERNEL_MODEL_
	FileObject * fptr = open_file(path, FILE_MODE_ALL);
	#else
	ILFILE * fptr = ILOpenFile(path, FILE_MODE_ALL);
	#endif
	if(fptr == NULL)
		return FALSE;
	switch(common_image->type)
	{
		case CMMNIMAGE_IMG0:
			#ifdef	_KERNEL_MODEL_
			write_file(fptr, CMMNIMAGE_IMG0_SYMBOL, strlen(CMMNIMAGE_IMG0_SYMBOL));			
			append_file(fptr, (uint8 *)&(common_image->width), 4);	
			append_file(fptr, (uint8 *)&(common_image->height), 4);
			append_file(fptr, common_image->data, common_image->width * common_image->height * 4);
			#else
			ILWriteFile(fptr, CMMNIMAGE_IMG0_SYMBOL, strlen(CMMNIMAGE_IMG0_SYMBOL));			
			ILAppendFile(fptr, (uint8 *)&(common_image->width), 4);	
			ILAppendFile(fptr, (uint8 *)&(common_image->height), 4);
			ILAppendFile(fptr, common_image->data, common_image->width * common_image->height * 4);
			#endif
			break;
		default:
			return FALSE;
	}
	#ifdef	_KERNEL_MODEL_
	close_file(fptr);
	#else
	ILCloseFile(fptr);
	#endif
	return TRUE;
}

/**
	@Function:		fill_image_by_byte
	@Access:		Public
	@Description:
		填充字节数据到图片。
	@Parameters:
		common_image, struct CommonImage *, IN OUT
			图片。
		data, uint8, IN
			字节数据。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
fill_image_by_byte(	IN OUT struct CommonImage * common_image,
					IN uint8 data)
{
	if(common_image == NULL || common_image->data == NULL)
		return FALSE;
	uint32 ui;
	uint32 len = common_image->width * common_image->height * 4;
	image_fast_fill_uint8(common_image->data, data, len);
	return TRUE;
}

/**
	@Function:		set_pixel_common_image
	@Access:		Public
	@Description:
		设置图片的像素。
	@Parameters:
		common_image, struct CommonImage *, IN OUT
			图片。
		x, int32, IN
			X 坐标。
		y, int32, IN
			Y 坐标。
		pixel, uint32, IN
			像素值。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。			
*/
BOOL
set_pixel_common_image(	IN OUT struct CommonImage * common_image,
						IN int32 x,
						IN int32 y,
						IN uint32 pixel)
{
	switch(common_image->type)
	{
		case CMMNIMAGE_IMG0:
		{
			if(common_image == NULL || x >= common_image->width || y >= common_image->height || x < 0 || y < 0)
				return FALSE;
			uint32 * data_ptr = common_image->data;
			data_ptr[y * common_image->width + x] = pixel;
			return TRUE;
		}
		default:
			return FALSE;
	}
}

/**
	@Function:		get_pixel_image0
	@Access:		Public
	@Description:
		获取 Image0 图片的像素。
	@Parameters:
		common_image, struct CommonImage *, IN
			Image0 图片。
		x, int32, IN
			X 坐标。
		y, int32, IN
			Y 坐标。
	@Return:
		uint32
			指定坐标的像素值。如果指定的坐标超出图片的范围，则返回RGB(0, 0, 0)。		
*/
static
uint32
get_pixel_image0(	IN struct CommonImage * common_image,
					IN int32 x,
					IN int32 y)
{
	if(common_image == NULL || x >= common_image->width || y >= common_image->height || x < 0 || y < 0)
		return IMAGE_PIXEL_RGB(0x00, 0x00, 0x00);
	uint32 * data_ptr = common_image->data;
	return data_ptr[y * common_image->width + x];
}

/**
	@Function:		get_pixel_common_image
	@Access:		Public
	@Description:
		获取图片的像素。
	@Parameters:
		common_image, struct CommonImage *, IN
			图片。
		x, int32, IN
			X 坐标。
		y, int32, IN
			Y 坐标。
	@Return:
		uint32
			指定坐标的像素值。如果指定的坐标超出图片的范围，则返回RGB(0, 0, 0)。		
*/
uint32
get_pixel_common_image(	IN struct CommonImage * common_image,
						IN int32 x,
						IN int32 y)
{
	switch(common_image->type)
	{
		case CMMNIMAGE_IMG0:
			return get_pixel_image0(common_image, x, y);
		default:
			return IMAGE_PIXEL_RGB(0x00, 0x00, 0x00);
	}
}

/**
	@Function:		draw_image0
	@Access:		Public
	@Description:
		把一张 Image0 图片画到另一张 Image0 图片上。
	@Parameters:
		dst, struct CommonImage *, IN OUT
			目标的 Image0 图片。
		src, struct CommonImage *, IN
			源的 Image0 图片。
		draw_x, int32, IN
			目标 X 坐标。
		draw_y, int32, IN
			目标 Y 坐标。
		width, int32, IN
			源的 Image0 图片的最大宽度。
		height, int32, IN
			源的 Image0 图片的最大高度。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
draw_image0(IN OUT struct CommonImage * dst,
			IN struct CommonImage * src,
			IN int32 draw_x,
			IN int32 draw_y,
			IN int32 width,
			IN int32 height)
{
	if(	dst == NULL 
		|| src == NULL 
		|| width == 0 
		|| height == 0 
		|| dst->type != CMMNIMAGE_IMG0 
		|| src->type != CMMNIMAGE_IMG0)
		return FALSE;
	int32 dst_x, dst_y;
	int32 src_x, src_y;
	uint32 * dst_ptr = dst->data;
	uint32 * src_ptr = src->data;
	int32 draw_x_width = draw_x + width;
	int32 draw_y_height = draw_y + height;
	int32 dst_width = (int32)dst->width;
	int32 src_width = (int32)src->width;
	for(dst_y = draw_y, src_y = 0; dst_y < draw_y_height; dst_y++, src_y++)
	{
		if(	dst_y >= dst->height
			|| src_y >= height
			|| src_y >= src->height
			|| dst_y < 0)
			continue;
		int32 dstoff = dst_y * dst_width;
		int32 srcoff = src_y * src_width;
		for(dst_x = draw_x, src_x = 0; dst_x < draw_x_width; dst_x++, src_x++)
		{
			if(	dst_x >= dst->width
				|| src_x >= width
				|| src_x >= src->width
				|| dst_x < 0)
				continue;
			dst_ptr[dstoff + dst_x] = src_ptr[srcoff + src_x];
		}
	}
	return TRUE;
}

/**
	@Function:		draw_common_image
	@Access:		Public
	@Description:
		把一张图片画到另一张图片上。
	@Parameters:
		dst, struct CommonImage *, IN OUT
			目标的图片。
		src, struct CommonImage *, IN
			源的图片。
		draw_x, int32, IN
			目标 X 坐标。
		draw_y, int32, IN
			目标 Y 坐标。
		width, int32, IN
			源的图片的最大宽度。
		height, int32, IN
			源的图片的最大高度。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
draw_common_image(	IN OUT struct CommonImage * dst,
					IN struct CommonImage * src,
					IN int32 draw_x,
					IN int32 draw_y,
					IN int32 width,
					IN int32 height)
{
	if(dst->type == CMMNIMAGE_IMG0 && src->type == CMMNIMAGE_IMG0)
		return draw_image0(dst, src, draw_x, draw_y, width, height);
	else
		return FALSE;
}

/**
	@Function:		draw_image0_t
	@Access:		Public
	@Description:
		把一张 Image0 图片画到另一张 Image0 图片上。
		并且可以指定某像素为透明像素，不会被画到目标 Image0 图片上。
	@Parameters:
		dst, struct CommonImage *, IN OUT
			目标的 Image0 图片。
		src, struct CommonImage *, IN
			源的 Image0 图片。
		draw_x, int32, IN
			目标 X 坐标。
		draw_y, int32, IN
			目标 Y 坐标。
		width, int32, IN
			源的 Image0 图片的最大宽度。
		height, int32, IN
			源的 Image0 图片的最大高度。
		tpixel, uint32, IN
			透明的像素。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
draw_image0_t(	IN OUT struct CommonImage * dst, 
				IN struct CommonImage * src,
				IN int32 draw_x,
				IN int32 draw_y,
				IN int32 width,
				IN int32 height,
				IN uint32 tpixel)
{
	if(dst == NULL || src == NULL || width == 0 || height == 0 || dst->type != CMMNIMAGE_IMG0 || src->type != CMMNIMAGE_IMG0)
		return FALSE;
	int32 dst_x, dst_y;
	int32 src_x, src_y;
	uint32 * dst_ptr = dst->data;
	uint32 * src_ptr = src->data;
	for(dst_x = draw_x, src_x = 0; dst_x < draw_x + width; dst_x++, src_x++)
		for(dst_y = draw_y, src_y = 0; dst_y < draw_y + height; dst_y++, src_y++)
		{
			if(	dst_x >= dst->width 
				|| dst_y >= dst->height 
				|| src_x >= width 
				|| src_y >= height
				|| src_x >= src->width
				|| src_y >= src->height
				|| dst_x < 0
				|| dst_y < 0)
				continue;
			uint32 pixel = src_ptr[src_y * src->width + src_x];
			if((tpixel & 0x00ffffff) != (pixel & 0x00ffffff))
				dst_ptr[dst_y * dst->width + dst_x] = pixel;
		}
	return TRUE;
}

/**
	@Function:		draw_common_image_t
	@Access:		Public
	@Description:
		把一张图片画到另一张图片上。
		并且可以指定某像素为透明像素，不会被画到目标图片上。
	@Parameters:
		dst, struct CommonImage *, IN OUT
			目标的图片。
		src, struct CommonImage *, IN
			源的图片。
		draw_x, int32, IN
			目标 X 坐标。
		draw_y, int32, IN
			目标 Y 坐标。
		width, int32, IN
			源的图片的最大宽度。
		height, int32, IN
			源的图片的最大高度。
		tpixel, uint32, IN
			透明的像素。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
draw_common_image_t(IN OUT struct CommonImage * dst, 
					IN struct CommonImage * src, 
					IN int32 draw_x, 
					IN int32 draw_y, 
					IN int32 width, 
					IN int32 height, 
					IN uint32 tpixel)
{
	if(dst->type == CMMNIMAGE_IMG0 && dst->type == CMMNIMAGE_IMG0)
		return draw_image0_t(dst, src, draw_x, draw_y, width, height, tpixel);
	else
		return FALSE;
}

/**
	@Function:		hline_common_image
	@Access:		Public
	@Description:
		画一条水平线到图片上。
	@Parameters:
		common_image, struct CommonImage *, IN OUT
			图片。
		start_x, int32, IN
			起始 X 坐标。
		end_x, int32, IN
			起始 Y 坐标。
		length, int32, IN
			线的长度。
		pixel, uint32, IN
			线的像素值。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
hline_common_image(	IN OUT struct CommonImage * common_image,
					IN int32 start_x,
					IN int32 start_y,
					IN int32 length,
					IN uint32 pixel)
{
	if(common_image == NULL || start_y >= (int32)common_image->height)
		return FALSE;
	int32 x;
	uint32 * data_ptr = common_image->data;
	for(x = 0; x < length && x + start_x < (int32)common_image->width; x++)
		if(start_x + x >= 0 && start_y >= 0)
			data_ptr[start_y * common_image->width + start_x + x] = pixel;
	return TRUE;
}

/**
	@Function:		vline_common_image
	@Access:		Public
	@Description:
		画一条垂直线到图片上。
	@Parameters:
		common_image, struct CommonImage *, IN OUT
			图片。
		start_x, int32, IN
			起始 X 坐标。
		end_x, int32, IN
			起始 Y 坐标。
		length, int32, IN
			线的长度。
		pixel, uint32, IN
			线的像素值。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
vline_common_image(	IN OUT struct CommonImage * common_image,
					IN int32 start_x,
					IN int32 start_y,
					IN int32 length,
					IN uint32 pixel)
{
	if(common_image == NULL || start_x >= (int32)common_image->width)
		return FALSE;
	int32 y;
	uint32 * data_ptr = common_image->data;
	for(y = 0; y < length && y + start_y < (int32)common_image->height; y++)
		if(start_y + y >= 0 && start_x >= 0)
			data_ptr[(start_y + y) * common_image->width + start_x] = pixel;
	return TRUE;
}

/**
	@Function:		line_common_image
	@Access:		Public
	@Description:
		画线到图片上。
	@Parameters:
		common_image, struct CommonImage *, OUT
			图片。
		start_x, int32, IN
			起始 X 坐标。
		start_x, int32, IN
			起始 Y 坐标。
		end_x, int32, IN
			结束 X 坐标。
		end_y, int32, IN
			结束 Y 坐标。
		pixel, uint32, IN
			线的像素值。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
line_common_image(	OUT struct CommonImage * common_image,
					IN int32 start_x,
					IN int32 start_y,
					IN int32 end_x,
					IN int32 end_y,
					IN uint32 pixel)
{
	int32 middle_x = (start_x + end_x) / 2;
	int32 middle_y = (start_y + end_y) / 2;
	if((start_x - end_x) * (start_x - end_x) + (start_y - end_y) * (start_y - end_y) <= 2)
		set_pixel_common_image(common_image, start_x, start_y, pixel);
	else
	{
		line_common_image(common_image, start_x, start_y, middle_x, middle_y, pixel);
		line_common_image(common_image, middle_x, middle_y, end_x, end_y, pixel);
	}
	return TRUE;
}

/**
	@Function:		rect_common_image
	@Access:		Public
	@Description:
		画一个矩形到图片。
	@Parameters:
		common_image, struct CommonImage *, OUT
			图片。
		start_x, int32, IN
			起始 X 坐标。
		start_x, int32, IN
			起始 Y 坐标。
		width, int32, IN
			矩形的宽度。
		height, int32, IN
			矩形的高度。
		pixel, uint32, IN
			矩形填充的像素值。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
rect_common_image(	OUT struct CommonImage * common_image,
					IN int32 start_x,
					IN int32 start_y,
					IN int32 width,
					IN int32 height,
					IN uint32 pixel)
{
	if(common_image == NULL || width <= 0 || height <= 0)
		return FALSE;
	int32 x, y;
	for(x = start_x; x < start_x + width; x++)
		for(y = start_y; y < start_y + height; y++)
			if(x < common_image->width && y < common_image->height)
				set_pixel_common_image(common_image, x, y, pixel);
	return TRUE;
}

/**
	@Function:		blend
	@Access:		Private
	@Description:
		Alpha混合两个像素。
	@Parameters:
		back, uint32, IN
			位于后方的像素的颜色值。
		fore, uint32, IN
			位于前方的像素的颜色值。
		alpha, uint32, IN
			Alpha值。
	@Return:
		uint32
			混合完的像素值。	
*/
static
uint32
blend(	IN uint32 back,
		IN uint32 fore,
		IN uint32 alpha)
{
	uint32 mask_full = 0xFFFFFFFF;
	uint32 result;

	__asm__ __volatile__ (
		"PXOR %%xmm7, %%xmm7;"
		"MOVD %1, %%xmm0;"  // back
		"MOVD %2, %%xmm1;"  // fore
		"MOVD %3, %%xmm2;"  // alpha
		"MOVD %4, %%xmm3;"  // 0xFFFFFFFF
		"PUNPCKLBW %%xmm7, %%xmm0;"
		"PUNPCKLBW %%xmm7, %%xmm1;"
		"PUNPCKLBW %%xmm2, %%xmm2;"
		"PUNPCKLWD %%xmm2, %%xmm2;"
		"PUNPCKLBW %%xmm7, %%xmm2;"
		"PUNPCKLBW %%xmm7, %%xmm3;"
		"PMULLW %%xmm2, %%xmm1;"  // xmm1 = fore * alpha
		"PSUBW %%xmm2, %%xmm3;"   // xmm3 = 255 - alpha
		"PMULLW %%xmm3, %%xmm0;"  // xmm0 = back * (255 - alpha)
		"PADDW %%xmm1, %%xmm0;"   // xmm0 = xmm0 + xmm1
		"PSRLW $8, %%xmm0;"       // xmm0 = xmm0 >> 8
		"PACKUSWB %%xmm0, %%xmm0;"
		"MOVD %%xmm0, %0;"
		: "=m"(result)
		: "m"(back), "m"(fore), "m"(alpha), "m"(mask_full)
	);

	return result;
}

/**
	@Function:		blendx_init
	@Access:		Private
	@Description:
		在使用blendx(...)函数之前必须先调用该函数。
	@Parameters:
	@Return:
*/
static
void
blendx_init()
{
	__asm__ (
		"PXOR %%xmm7, %%xmm7;"
		"MOV $0x01000100, %%ebx;"
		"MOVD %%ebx, %%xmm6;"
		"PUNPCKLDQ %%xmm6, %%xmm6;"
		"PUNPCKLQDQ %%xmm6, %%xmm6;"
		:
		:
	);
}

/**
	@Function:		blendx
	@Access:		Private
	@Description:
		一次Alpha混合两组像素。
		在调用该函数前必须调用blendx_init()。
	@Parameters:
		back, uint32 *, IN
			指向储存着位于后方的两个像素的颜色值的缓冲区。
		fore, uint32 *, IN
			指向储存着位于前方的两个像素的颜色值的缓冲区。
		alpha, uint8 *, IN
			指向储存着两个Alpha值的缓存区。
		result, uint32 *, OUT
			指向一个能保存两个uitn32的缓冲区。
	@Return:	
*/
static
void
blendx(	IN uint32 * back,
		IN uint32 * fore,
		IN uint8 * alpha,
		OUT uint32 * result)
{
	__asm__ (
		"MOV %1, %%ebx;"
		"MOVQ (%%ebx), %%xmm0;"   // back
		"MOV %2, %%ebx;"
		"MOVQ (%%ebx), %%xmm1;"   // fore
		"XOR %%ebx, %%ebx;"
		"MOV (%3), %%bx;"
		"MOVD %%ebx, %%xmm2;"   // alpha
		"PUNPCKLBW %%xmm7, %%xmm0;"
		"PUNPCKLBW %%xmm7, %%xmm1;"
		"PUNPCKLBW %%xmm2, %%xmm2;"
		"PUNPCKLWD %%xmm2, %%xmm2;"
		"PUNPCKLBW %%xmm7, %%xmm2;"
		"MOVDQA %%xmm6, %%xmm3;"
		"PMULLW %%xmm2, %%xmm1;"  // xmm1 = fore * alpha
		"PSUBW %%xmm2, %%xmm3;"   // xmm3 = 256 - alpha
		"PMULLW %%xmm3, %%xmm0;"  // xmm0 = back * (256 - alpha)
		"PADDW %%xmm1, %%xmm0;"   // xmm0 = xmm0 + xmm1
		"PSRLW $8, %%xmm0;"       // xmm0 = xmm0 >> 8
		"PACKUSWB %%xmm0, %%xmm0;"
		"MOV %0, %%ebx;"
		"MOVQ %%xmm0, (%%ebx);"
		: "=m"(result)
		: "m"(back), "m"(fore), "p"(alpha)
	);
}

/**
	@Function:		text_common_image_normal
	@Access:		Private
	@Description:
		渲染一段文本。
		这个版本未使用SSE，而是直接使用无符号整形进行Alpha混合计算。
	@Parameters:
		common_image, struct CommonImage *, OUT
			图片。
		draw_x, int32, IN
			起始位置 X 坐标。
		draw_y, int32, IN
			起始位置 Y 坐标。
		enfont, uint8 *, IN
			英文字体数据。
		text, int8 *, IN
			文本。
		count, uint32, IN
			文本中的字符数。
		color, uint32, IN
			文本颜色。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。			
*/
static
BOOL
text_common_image_normal(	OUT struct CommonImage * common_image,
							IN int32 draw_x,
							IN int32 draw_y,
							IN uint8 * enfont,
							IN int8 * text,
							IN uint32 count,
							IN uint32 color)
{
	if(common_image == NULL || enfont == NULL || text == NULL)
		return FALSE;
	uint32 char_count = strlen(text);
	uint32 ui;
	#ifdef	_KERNEL_MODEL_
	BOOL enfx_enabled = enfontx_enabled();
	uint8 * enfont_content = enfont + 6;
	uint32 enfont_size = ENFONT_WIDTH * ENFONT_HEIGHT;
	#endif
	for(ui = 0; ui < char_count && ui < count; ui++)
	{
		#ifdef	_KERNEL_MODEL_
		if(enfx_enabled)
		{
			uint8 * char_image = enfont_content + text[ui] * enfont_size;
			int32 font_x, font_y;
			for(font_y = 0; font_y < ENFONT_HEIGHT; font_y++)
			{
				uint8 * font_pixels = char_image + font_y * ENFONT_WIDTH;
				for(font_x = 0; font_x < ENFONT_WIDTH; font_x++)
				{
					uint32 pixel = (uint32)get_pixel_common_image(common_image, draw_x + font_x, draw_y + font_y);
					uint32 font_pixel = font_pixels[font_x];

					uint32 r2 = (color >> 16) & 0x000000ff;
					uint32 g2 = (color >> 8) & 0x000000ff;
					uint32 b2 = color & 0x000000ff;
					uint32 r1 = (pixel >> 16) & 0x000000ff;
					uint32 g1 = (pixel >> 8) & 0x000000ff;
					uint32 b1 = pixel & 0x000000ff;
					uint32 a = font_pixel;
					uint32 r = 0, g = 0, b = 0;
					uint32 a1 = 256 - a;
					r = (r1 * a1 + r2 * a) >> 8;
					g = (g1 * a1 + g2 * a) >> 8;
					b = (b1 * a1 + b2 * a) >> 8;
					uint32 result = (r << 16) | (g << 8) | b | 0xff000000;

					set_pixel_common_image(common_image, draw_x + font_x, draw_y + font_y, result);
				}
			}
		}
		else
		{
			uint8 * char_image = enfont + 6 + text[ui] * 16;
			int32 font_x, font_y;
			for(font_y = 0; font_y < ENFONT_HEIGHT; font_y++)
			{
				uint8 row = char_image[font_y];
				for(font_x = 0; font_x < ENFONT_WIDTH; font_x++)
					if(((row >> font_x) & 0x01))
						set_pixel_common_image(common_image, draw_x + font_x, draw_y + font_y, color);
			}
		}
		#else
		uint8 * char_image = enfont + 6 + text[ui] * 16;
		int32 font_x, font_y;
		for(font_y = 0; font_y < ENFONT_HEIGHT; font_y++)
		{
			uint8 row = char_image[font_y];
			for(font_x = 0; font_x < ENFONT_WIDTH; font_x++)
				if(((row >> font_x) & 0x01))
					set_pixel_common_image(common_image, draw_x + font_x, draw_y + font_y, color);
		}
		#endif
		draw_x += ENFONT_WIDTH;
	}
	return TRUE;
}

/**
	@Function:		text_common_image_sse
	@Access:		Private
	@Description:
		渲染一段文本。
		这个版本使用了SSE。
	@Parameters:
		common_image, struct CommonImage *, OUT
			图片。
		draw_x, int32, IN
			起始位置 X 坐标。
		draw_y, int32, IN
			起始位置 Y 坐标。
		enfont, uint8 *, IN
			英文字体数据。
		text, int8 *, IN
			文本。
		count, uint32, IN
			文本中的字符数。
		color, uint32, IN
			文本颜色。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。			
*/
static
BOOL
text_common_image_sse(	OUT struct CommonImage * common_image,
						IN int32 draw_x,
						IN int32 draw_y,
						IN uint8 * enfont,
						IN int8 * text,
						IN uint32 count,
						IN uint32 color)
{
	if(common_image == NULL || enfont == NULL || text == NULL)
		return FALSE;
	uint32 char_count = strlen(text);
	uint32 ui;
	#ifdef	_KERNEL_MODEL_
	BOOL enfx_enabled = enfontx_enabled();
	uint8 * enfont_content = enfont + 6;
	uint32 enfont_size = ENFONT_WIDTH * ENFONT_HEIGHT;
	#endif
	for(ui = 0; ui < char_count && ui < count; ui++)
	{
		#ifdef	_KERNEL_MODEL_
		if(enfx_enabled)
		{
			uint8 * char_image = enfont_content + text[ui] * enfont_size;
			int32 font_x, font_y;
			for(font_y = 0; font_y < ENFONT_HEIGHT; font_y++)
			{
				uint8 * font_pixels = char_image + font_y * ENFONT_WIDTH;
				for(font_x = 0; font_x < ENFONT_WIDTH; font_x++)
				{
					uint32 pixel = (uint32)get_pixel_common_image(common_image, draw_x + font_x, draw_y + font_y);
					uint32 font_pixel = font_pixels[font_x];
					uint32 result = blend(pixel, color, font_pixel);
					set_pixel_common_image(common_image, draw_x + font_x, draw_y + font_y, result);
				}
			}
		}
		else
		{
			uint8 * char_image = enfont + 6 + text[ui] * 16;
			int32 font_x, font_y;
			for(font_y = 0; font_y < ENFONT_HEIGHT; font_y++)
			{
				uint8 row = char_image[font_y];
				for(font_x = 0; font_x < ENFONT_WIDTH; font_x++)
					if(((row >> font_x) & 0x01))
						set_pixel_common_image(common_image, draw_x + font_x, draw_y + font_y, color);
			}
		}
		#else
		uint8 * char_image = enfont + 6 + text[ui] * 16;
		int32 font_x, font_y;
		for(font_y = 0; font_y < ENFONT_HEIGHT; font_y++)
		{
			uint8 row = char_image[font_y];
			for(font_x = 0; font_x < ENFONT_WIDTH; font_x++)
				if(((row >> font_x) & 0x01))
					set_pixel_common_image(common_image, draw_x + font_x, draw_y + font_y, color);
		}
		#endif
		draw_x += ENFONT_WIDTH;
	}
	return TRUE;
}

/**
	@Function:		text_common_image_sse_ex
	@Access:		Private
	@Description:
		渲染一段文本。
		这个版本使用了SSE。使用了blendex(...)来进行Alpha混合计算。
	@Parameters:
		common_image, struct CommonImage *, OUT
			图片。
		draw_x, int32, IN
			起始位置 X 坐标。
		draw_y, int32, IN
			起始位置 Y 坐标。
		enfont, uint8 *, IN
			英文字体数据。
		text, int8 *, IN
			文本。
		count, uint32, IN
			文本中的字符数。
		color, uint32, IN
			文本颜色。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。			
*/
static
BOOL
text_common_image_sse_ex(	OUT struct CommonImage * common_image,
							IN int32 draw_x,
							IN int32 draw_y,
							IN uint8 * enfont,
							IN int8 * text,
							IN uint32 count,
							IN uint32 color)
{
	if(common_image == NULL || enfont == NULL || text == NULL)
		return FALSE;
	uint32 char_count = strlen(text);
	uint32 ui;
	#ifdef	_KERNEL_MODEL_
	BOOL enfx_enabled = enfontx_enabled();
	uint8 * enfont_content = enfont + 6;
	uint32 enfont_size = ENFONT_WIDTH * ENFONT_HEIGHT;
	uint32 half_enfont_width = ENFONT_WIDTH / 2;
	#endif
	blendx_init();
	for(ui = 0; ui < char_count && ui < count; ui++)
	{
		#ifdef	_KERNEL_MODEL_
		if(enfx_enabled)
		{
			uint8 * char_image = enfont_content + text[ui] * enfont_size;
			int32 font_x, font_y;
			for(font_y = 0; font_y < ENFONT_HEIGHT; font_y++)
			{
				uint8 * font_pixels = char_image + font_y * ENFONT_WIDTH;
				for(font_x = 0; font_x < half_enfont_width; font_x++)
				{
					uint32 back[2] __attribute__((aligned(16)));
					uint32 fore[2] __attribute__((aligned(16)));
					uint8 alpha[2] __attribute__((aligned(16)));
					uint32 result[2] __attribute__((aligned(16)));
					fore[0] = color;
					fore[1] = color;
					uint32 temp = draw_x + font_x * 2;
					back[0] = get_pixel_common_image(common_image, temp, draw_y + font_y);
					back[1] = get_pixel_common_image(common_image, temp + 1, draw_y + font_y);
					uint8 * tempu8 = char_image + font_y * ENFONT_WIDTH + font_x * 2;
					alpha[0] = *tempu8;
					alpha[1] = *(tempu8 + 1);
					blendx(back, fore, alpha, result);
					temp = draw_x + font_x * 2;
					set_pixel_common_image(common_image, temp, draw_y + font_y, result[0]);
					set_pixel_common_image(common_image, temp + 1, draw_y + font_y, result[1]);
				}
			}
		}
		else
		{
			uint8 * char_image = enfont + 6 + text[ui] * 16;
			int32 font_x, font_y;
			for(font_y = 0; font_y < ENFONT_HEIGHT; font_y++)
			{
				uint8 row = char_image[font_y];
				for(font_x = 0; font_x < ENFONT_WIDTH; font_x++)
					if(((row >> font_x) & 0x01))
						set_pixel_common_image(common_image, draw_x + font_x, draw_y + font_y, color);
			}
		}
		#else
		uint8 * char_image = enfont + 6 + text[ui] * 16;
		int32 font_x, font_y;
		for(font_y = 0; font_y < ENFONT_HEIGHT; font_y++)
		{
			uint8 row = char_image[font_y];
			for(font_x = 0; font_x < ENFONT_WIDTH; font_x++)
				if(((row >> font_x) & 0x01))
					set_pixel_common_image(common_image, draw_x + font_x, draw_y + font_y, color);
		}
		#endif
		draw_x += ENFONT_WIDTH;
	}
	return TRUE;
}

/**
	@Function:		text_common_image
	@Access:		Public
	@Description:
		画文本到图片。
	@Parameters:
		common_image, struct CommonImage *, OUT
			图片。
		draw_x, int32, IN
			起始位置 X 坐标。
		draw_y, int32, IN
			起始位置 Y 坐标。
		enfont, uint8 *, IN
			英文字体数据。
		text, int8 *, IN
			文本。
		count, uint32, IN
			文本中的字符数。
		color, uint32, IN
			文本颜色。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。			
*/
BOOL
text_common_image(	OUT struct CommonImage * common_image,
					IN int32 draw_x,
					IN int32 draw_y,
					IN uint8 * enfont,
					IN int8 * text,
					IN uint32 count,
					IN uint32 color)
{
	switch(image_mode)
	{
		case IMAGE_MODE_SSE:
			return text_common_image_sse(	common_image,
											draw_x,
											draw_y,
											enfont,
											text,
											count,
											color);
		case IMAGE_MODE_SSE_EX:
			return text_common_image_sse_ex(common_image,
											draw_x,
											draw_y,
											enfont,
											text,
											count,
											color);
		case IMAGE_MODE_NORMAL:
		default:
			return text_common_image_normal(common_image,
											draw_x,
											draw_y,
											enfont,
											text,
											count,
											color);
	}
}

/**
	@Function:		text_common_image_ml
	@Access:		Public
	@Description:
		画文本到图片。支持换行的版本。
	@Parameters:
		common_image, struct CommonImage *, OUT
			图片。
		draw_x, int32, IN
			起始位置 X 坐标。
		draw_y, int32, IN
			起始位置 Y 坐标。
		enfont, uint8 *, IN
			英文字体数据。
		text, int8 *, IN
			文本。
		count, uint32, IN
			文本中的字符数。
		color, uint32, IN
			文本颜色。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。			
*/
BOOL
text_common_image_ml(	OUT struct CommonImage * common_image,
						IN int32 draw_x,
						IN int32 draw_y,
						IN uint8 * enfont,
						IN int8 * text,
						IN uint32 count,
						IN uint32 color)
{
	uint32 c = 0;
	uint32 len = strlen(text);
	uint32 ui;
	int8 * s = text;
	for(ui = 0; ui < len; ui++)
		if(text[ui] != '\n')
			c++;
		else
		{
			if(!text_common_image(	common_image,
									draw_x, draw_y,
									enfont, s, c, color))
				return FALSE;
			s += c + 1;
			draw_y += ENFONT_HEIGHT;
			c = 0;
		}
	if(c != 0)
		if(!text_common_image(	common_image,
								draw_x, draw_y,
								enfont, s, c, color))
			return FALSE;
	return TRUE;
}
