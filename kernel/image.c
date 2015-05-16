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
	#include "memory.h"
	#include "ifs1fs.h"
	#include <ilib/string.h>
	#define	ALLOCM(n)	(alloc_memory((n)))
	#define	FREEM(ptr)	(free_memory((ptr)))
#else
	#include <ilib/ilib.h>
	#define	ALLOCM(n)	(allocm((n)))
	#define	FREEM(ptr)	(freem((ptr)))
#endif

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
	FILE * fptr = fopen(path, FILE_MODE_ALL);
	#else
	ILFILE * fptr = ILOpenFile(path, FILE_MODE_ALL);
	#endif
	if(fptr == NULL)
		return FALSE;
	switch(common_image->type)
	{
		case CMMNIMAGE_IMG0:
			#ifdef	_KERNEL_MODEL_
			fwrite(fptr, CMMNIMAGE_IMG0_SYMBOL, strlen(CMMNIMAGE_IMG0_SYMBOL));			
			fappend(fptr, (uint8 *)&(common_image->width), 4);	
			fappend(fptr, (uint8 *)&(common_image->height), 4);
			fappend(fptr, common_image->data, common_image->width * common_image->height * 4);
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
	fclose(fptr);
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
	for(ui = 0; ui < len; ui++)
		common_image->data[ui] = data;
	return TRUE;
}

/**
	@Function:		set_pixel_image0
	@Access:		Public
	@Description:
		设置 Image0 图片的像素。
	@Parameters:
		common_image, struct CommonImage *, IN OUT
			Image0 图片。
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
static
BOOL
set_pixel_image0(	IN OUT struct CommonImage * common_image,
					IN int32 x,
					IN int32 y,
					IN uint32 pixel)
{
	if(common_image == NULL || x >= common_image->width || y >= common_image->height || x < 0 || y < 0)
		return FALSE;
	uint32 * data_ptr = common_image->data;
	data_ptr[y * common_image->width + x] = pixel;
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
			return set_pixel_image0(common_image, x, y, pixel);
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
					IN uint32 x,
					IN uint32 y)
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
						IN uint32 x,
						IN uint32 y)
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
			dst_ptr[dst_y * dst->width + dst_x] = src_ptr[src_y * src->width + src_x];
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
	if(common_image == NULL || start_y >= common_image->height)
		return FALSE;
	int32 x;
	uint32 * data_ptr = common_image->data;
	for(x = 0; x < length && x + start_x < common_image->width; x++)
		if(start_x + x >= 0)
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
	if(common_image == NULL || start_x >= common_image->width)
		return FALSE;
	int32 y;
	uint32 * data_ptr = common_image->data;
	for(y = 0; y < length && y + start_y < common_image->height; y++)
		if(start_y + y >= 0)
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
	if(common_image == NULL || enfont == NULL || text == NULL)
		return FALSE;
	uint32 char_count = strlen(text);
	uint32 ui;	
	for(ui = 0; ui < char_count && ui < count; ui++)
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
		draw_x += ENFONT_WIDTH;
	}
	return TRUE;
}
