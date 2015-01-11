/**
	@File:			stdio.c
	@Author:		Ihsoh
	@Date:			2014-1-19
	@Description:
		提供标准输入输出功能。
*/

#include "ilib.h"

int vsprintf_s(char * buffer, uint size, const char * format, va_list va)
{
	char chr;
	int buf_len = 0;
	int end = 0;
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
							strncpy(buffer, d_s, d_s_len);
							buffer += d_s_len;
							buf_len += d_s_len;
						}
						break;
					}
					case 'f':
					{
						float f = va_arg(va, float);
						char temp[100];
						char * f_s = dtos(temp, (double)f);
						uint f_s_len = strlen(f_s);
						if(buf_len + f_s_len >= size)
							end = 1;
						else
						{
							strncpy(buffer, f_s, f_s_len);
							buffer += f_s_len;
							buf_len += f_s_len;
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
							strncpy(buffer, x_s, x_s_len);
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
							strncpy(buffer, x_s, x_s_len);
							buffer += x_s_len;
							buf_len += x_s_len;
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

int sprintf_s(char * buffer, uint size, const char * format, ...)
{
	va_list va;
	va_start(va, format);
	int r = vsprintf_s(buffer, size, format, va);
	va_end(va);
	return r;
}

int sprintf(char * buffer, const char * format, ...)
{
	va_list va;
	va_start(va, format);
	int r = vsprintf_s(buffer, 0xffffffff, format, va);
	va_end(va);
	return r;
}

int printf(const char * format, ...)
{
	char buffer[10 * 1024];
	va_list va;
	va_start(va, format);
	int r = vsprintf_s(buffer, 0xffffffff, format, va);
	va_end(va);
	print_str(buffer);
	return r;
}
