/**
	@File:			kstring.c
	@Author:		Ihsoh
	@Date:			2015-2-10
	@Description:
		字符串处理。
*/

#include "kstring.h"
#include "types.h"

/**
	@Function:		split_string
	@Access:		Public
	@Description:
		分割字符串。
	@Parameters:
		dest, int8 *, OUT
			保存结果的缓冲区。如果传入 NULL，则清除状态。
		src, const int8 *, IN
			源字符串。
		spt, int8, IN
			分隔符。
		max, int32, IN
			保存结果的缓冲区的大小。
	@Return:
		int8 *
			如果 dest 为 NULL 或到达结尾或 max 为 NULL，
			则返回值为NULL。		
*/
int8 *
split_string(	OUT int8 * dest, 
				IN const int8 * src,
				IN int8 spt,
				IN int32 max)
{
	static int32 pos;
	if(dest == NULL)
	{
		pos = 0;
		return NULL;
	}
	else
	{
		if(max <= 0 || src == NULL)
			return NULL;
		src += pos;
		int8 chr;
		int32 index = 0;
		while(	++pos
				&& (chr = *(src++)) != spt
				&& chr != '\0'
				&& index + 1 < max)
			dest[index++] = chr;
		if(index == 0 && chr == '\0')
			return NULL;
		else
		{
			if(chr == '\0')
				pos--;
			dest[index] = '\0';
			return dest;
		}
	}
}
