#include "bmp.h"
#include "../types.h"
#include "../fs/ifs1/fs.h"
#include <dslib/list.h>
#include <dslib/value.h>

static
int32
_BmpCTZ(IN int32 x)
{
	int32 cnt = 0;
	for(; ~x & 1; x >>= 1)
		cnt += 1;
	return cnt;
}

static
int32
_BmpShake(IN int32 mask)
{
	int32 cnt, shift = _BmpCTZ(mask);
	mask = (mask >> shift) + 1;
	if((cnt = _BmpCTZ(mask)) < 8)
		shift -= 8 - cnt;
	return shift;
}

static
void
_BmpApplyMask(	IN IMGLBMPPtr bmpobj,
				IN int32 color, 
				OUT int32 * r,
				OUT int32 * g,
				OUT int32 * b)
{
	*r = color & bmpobj->red_mask;
	*g = color & bmpobj->green_mask;
	*b = color & bmpobj->blue_mask;

	#define __shift(x, bit) x = (bit > 0 ? x >> bit : x << (-bit))
	__shift(*r, bmpobj->red_shift);
	__shift(*g, bmpobj->green_shift);
	__shift(*b, bmpobj->blue_shift);
	#undef	__shift
}

IMGLBMPPtr
BmpCreate(IN CASCTEXT file)
{
	IMGLBMPPtr bmpobj = (IMGLBMPPtr)alloc_memory(sizeof(IMGLBMP));
	if(bmpobj == NULL)
		return NULL;

	bmpobj->imgfile_ptr = NULL;
	bmpobj->data_ptr = NULL;
	bmpobj->pixel_array_ptr = NULL;
	bmpobj->palette_r = NULL;
	bmpobj->palette_g = NULL;
	bmpobj->palette_b = NULL;

	bmpobj->palette_r = dsl_lst_new();
	if(bmpobj->palette_r == NULL)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}
	bmpobj->palette_g = dsl_lst_new();
	if(bmpobj->palette_g == NULL)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}
	bmpobj->palette_b = dsl_lst_new();
	if(bmpobj->palette_b == NULL)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}

	bmpobj->imgfile_ptr = Ifs1OpenFile(file, FILE_MODE_ALL);

	if(bmpobj->imgfile_ptr == NULL)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}

	bmpobj->data_ptr = NULL;
	bmpobj->width = 0;
	bmpobj->height = 0;

	uint8 head[0xe];
	int32 actual_size, pixel_array_offset;

	if((int32)Ifs1ReadFile(bmpobj->imgfile_ptr, head, 0xe) != 0xe)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}
	if(head[0] != 'B' || head[1] != 'M')
	{
		BmpDestroy(bmpobj);
		return NULL;
	}
	actual_size = (int32)*(uint32 *)(head + 0x2);
	pixel_array_offset = (int32)*(uint32 *)(head + 0xa);

	bmpobj->data_ptr = (uint8 *)alloc_memory(actual_size - 0xe);
	if(bmpobj->data_ptr == NULL)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}
	if((int32)Ifs1ReadFile(bmpobj->imgfile_ptr, bmpobj->data_ptr, actual_size - 0xe) != actual_size - 0xe)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}

	int32 header_size = (int32)*(uint32 *)bmpobj->data_ptr;
	
	if(header_size < 40)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}

	bmpobj->width = *(int32 *)(bmpobj->data_ptr + 0x4);
	if(bmpobj->width <= 0)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}

	bmpobj->height = *(int32 *)(bmpobj->data_ptr + 0x8);
	if(bmpobj->height == 0)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}

	bmpobj->color_depth = (int32)*(uint16 *)(bmpobj->data_ptr + 0xe);
	bmpobj->compression = (int32)*(uint32 *)(bmpobj->data_ptr + 0x10);

	if(bmpobj->compression != BI_RGB && bmpobj->compression != BI_BITFIELDS)
	{
		BmpDestroy(bmpobj);
		return NULL;
	}

	bmpobj->palette_size = (int32)*(uint32 *)(bmpobj->data_ptr + 0x20);

	if(bmpobj->compression == BI_RGB)
	{
		bmpobj->red_mask = 0xff0000;
		bmpobj->red_shift = 16;
		bmpobj->green_mask = 0xff00;
		bmpobj->green_shift = 8;
		bmpobj->blue_mask = 0xff;
		bmpobj->blue_shift = 0;
	}
	else if(bmpobj->compression == BI_BITFIELDS && header_size >= 56)
	{
		bmpobj->red_mask = (int32)*(uint32 *)(bmpobj->data_ptr + 0x28);
		bmpobj->green_mask = (int32)*(uint32 *)(bmpobj->data_ptr + 0x2c);
		bmpobj->blue_mask = (int32)*(uint32 *)(bmpobj->data_ptr + 0x30);
		bmpobj->red_shift = _BmpShake(bmpobj->red_mask);
		bmpobj->green_shift = _BmpShake(bmpobj->green_mask);
		bmpobj->blue_shift = _BmpShake(bmpobj->blue_mask);
	}
	else
	{
		BmpDestroy(bmpobj);
		return NULL;
	}

	if(bmpobj->color_depth <= 8 && bmpobj->palette_size == 0)
		bmpobj->palette_size = 1 << bmpobj->color_depth;

	if(bmpobj->palette_size)
	{
		uint32 * palette_ptr = (uint32 *)(bmpobj->data_ptr + header_size);
		int32 r, g, b;

		for(int32 i = 0; i < bmpobj->palette_size; ++i)
		{
			_BmpApplyMask(bmpobj, *palette_ptr, &r, &g, &b);
			DSLValuePtr val_r = dsl_val_int32(r);
			dsl_lst_add_value(bmpobj->palette_r, val_r); 
			DSLValuePtr val_g = dsl_val_int32(g);
			dsl_lst_add_value(bmpobj->palette_g, val_g); 
			DSLValuePtr val_b = dsl_val_int32(b);
			dsl_lst_add_value(bmpobj->palette_b, val_b); 
			palette_ptr += 1;
		}
	}

	bmpobj->pixel_array_ptr = bmpobj->data_ptr + pixel_array_offset - 0xe;

	bmpobj->row_size = (bmpobj->color_depth * bmpobj->width + 31) / 32 * 4;

	return bmpobj;
}
 
BOOL
BmpDestroy(IN IMGLBMPPtr bmpobj)
{
	if(bmpobj == NULL)
		return FALSE;
	if(bmpobj->data_ptr != NULL)
		free_memory(bmpobj->data_ptr);
	if(bmpobj->imgfile_ptr != NULL)
		Ifs1CloseFile(bmpobj->imgfile_ptr);
	if(bmpobj->palette_r != NULL)
	{
		dsl_lst_delete_all_value(bmpobj->palette_r);
		dsl_lst_free(bmpobj->palette_r);
	}
	if(bmpobj->palette_g != NULL)
	{
		dsl_lst_delete_all_value(bmpobj->palette_g);
		dsl_lst_free(bmpobj->palette_g);
	}
	if(bmpobj->palette_b != NULL)
	{
		dsl_lst_delete_all_value(bmpobj->palette_b);
		dsl_lst_free(bmpobj->palette_b);
	}
	free_memory(bmpobj);
	return TRUE;
}

int32
BmpGetWidth(IN IMGLBMPPtr bmpobj)
{
	if(bmpobj == NULL)
		return 0;
	return bmpobj->width;
}

int32
BmpGetHeight(IN IMGLBMPPtr bmpobj)
{
	if(bmpobj == NULL)
		return 0;
	return bmpobj->height > 0 ? bmpobj->height : -bmpobj->height;
}

uint32
BmpGetPixel(	IN IMGLBMPPtr bmpobj,
					IN int32 x, 
					IN int32 y)
{
	if(bmpobj == NULL)
		return 0;
	uchar * ptr = (uchar *)bmpobj->pixel_array_ptr + bmpobj->row_size
					* (bmpobj->height > 0 ? bmpobj->height - y - 1 : y);
	int32 bit_offset = (x * bmpobj->color_depth) % 8, byte_offset = (x * bmpobj->color_depth) / 8;
	int32 r, g, b, ret = 0;

	ptr += byte_offset;
	bit_offset = 8 - bmpobj->color_depth - bit_offset;

	if(bmpobj->color_depth <= 8)
		ret = (int32)(*ptr >> bit_offset) & ((1 << bmpobj->color_depth) - 1);
	else if(bmpobj->color_depth == 16)
		ret = (int32)*(uint16 *)ptr;
	else if(bmpobj->color_depth == 24 || bmpobj->color_depth == 36)
		ret = (int32)*(uint32 *)ptr;
	if(bmpobj->palette_size)
	{
		DSLValuePtr val_r = dsl_lst_get(bmpobj->palette_r, ret);
		DSLValuePtr val_g = dsl_lst_get(bmpobj->palette_g, ret);
		DSLValuePtr val_b = dsl_lst_get(bmpobj->palette_b, ret);
		return	(val_r->value.int32_value << 16)
				| (val_g->value.int32_value << 8)
				| val_b->value.int32_value;
	}
	else
	{
		_BmpApplyMask(bmpobj, ret, &r, &g, &b);
		return (uint32)((r << 16) | (g << 8) | b);
	}
}
