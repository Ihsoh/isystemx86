/**
	@File:			kstring.c
	@Author:		Ihsoh
	@Date:			2015-2-10
	@Description:
		字符串处理。
*/

#include "kstring.h"
#include "types.h"

#include <ilib/string.h>

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

/**
	@Function:		vsprintf_s
	@Access:		Public
	@Description:
		格式化字符串。
	@Parameters:
		buffer, int8 *, OUT
			保存结果的缓冲区。
		size, uint32, IN
			缓冲区大小。
		format, const int8 *, IN
			格式字符串。
		va, va_list, IN
			参数列表。
	@Return:
		int32
			字符串长度。		
*/
int32
vsprintf_s(	OUT int8 * buffer,
			IN uint32 size,
			IN const int8 * format,
			IN va_list va)
{
	int8 chr;
	uint32 buf_len = 0;
	int32 end = 0;
	while(!end && (chr = *(format++)) != '\0' && buf_len + 1 < size)
		if(chr != '%')
		{
			*(buffer++) = chr;
			buf_len++;
		}
		else
			if((chr = *(format++)) == '\0')
				break;
			else
				switch(chr)
				{
					case '%':
						if(buf_len + 1 >= size)
							end = 1;
						else
						{
							*(buffer++) = '%';
							buf_len++;
						}
						break;
					case 'c':
					{
						if(buf_len + 1 >= size)
							end = 1;
						else
						{
							char c = va_arg(va, char);
							*(buffer++) = c;
							buf_len++;
						}						
						break;
					}
					case 'd':
					{
						int d = va_arg(va, int);
						char temp[100];
						char * d_s = itos(temp, d);
						uint d_s_len = strlen(d_s);
						if(buf_len + d_s_len >= size)
							end = 1;
						else
						{
							memcpy_safe(buffer, d_s_len, d_s, d_s_len);
							buffer += d_s_len;
							buf_len += d_s_len;
						}
						break;
					}
					case 'f':
					case 'g':
					{
						double d = va_arg(va, double);
						char temp[100];
						char * d_s = dtos(temp, d);
						uint d_s_len = strlen(d_s);
						if(buf_len + d_s_len >= size)
							end = 1;
						else
						{
							memcpy_safe(buffer, d_s_len, d_s, d_s_len);
							buffer += d_s_len;
							buf_len += d_s_len;
						}
						break;
					}
					case 'x':
					{
						uint x = va_arg(va, int);
						char temp[100];
						char * x_s = uitohexs(temp, x);
						uint x_s_len = strlen(x_s);
						int i;
						for(i = 0; i < x_s_len; i++)
							if(temp[i] >= 'A' && temp[i] <= 'Z')
								temp[i] = 'a' + (temp[i] - 'A');
						if(buf_len + x_s_len >= size)
							end = 1;
						else
						{
							memcpy_safe(buffer, x_s_len, x_s, x_s_len);
							buffer += x_s_len;
							buf_len += x_s_len;
						}
						break;
					}
					case 'X':
					{
						uint x = va_arg(va, int);
						char temp[100];
						char * x_s = uitohexs(temp, x);
						uint x_s_len = strlen(x_s);
						if(buf_len + x_s_len >= size)
							end = 1;
						else
						{
							memcpy_safe(buffer, x_s_len, x_s, x_s_len);
							buffer += x_s_len;
							buf_len += x_s_len;
						}
						break;
					}
					case 's':
					{
						char * str = va_arg(va, char *);
						uint str_len = strlen(str);
						if(buf_len + str_len >= size)
							end = 1;
						else
						{
							memcpy_safe(buffer, str_len, str, str_len);
							buffer += str_len;
							buf_len += str_len;
						}
						break;
					}
					default:
						*(buffer++) = chr;
						buf_len++;
						break;
				}

	*buffer = '\0';
	return buf_len;
}

/**
	@Function:		sprintf_s
	@Access:		Public
	@Description:
		格式化字符串。
	@Parameters:
		buffer, int8 *, OUT
			保存结果的缓冲区。
		size, uint32, IN
			缓冲区大小。
		format, const int8 *, IN
			格式字符串。
		..., VALIST, IN
			参数列表。
	@Return:
		int32
			字符串长度。		
*/
int32
sprintf_s(	OUT int8 * buffer,
			IN uint32 size,
			IN const int8 * format,
			...)
{
	va_list va;
	va_start(va, format);
	int32 r = vsprintf_s(buffer, size, format, va);
	va_end(va);
	return r;
}

/**
	@Function:		sprintf
	@Access:		Public
	@Description:
		格式化字符串。
	@Parameters:
		buffer, int8 *, OUT
			保存结果的缓冲区。
		format, const int8 *, IN
			格式字符串。
		..., VALIST, IN
			参数列表。
	@Return:
		int32
			字符串长度。		
*/
int32
sprintf(OUT int8 * buffer,
		IN const int8 * format,
		...)
{
	va_list va;
	va_start(va, format);
	int32 r = vsprintf_s(buffer, 0xffffffff, format, va);
	va_end(va);
	return r;
}
