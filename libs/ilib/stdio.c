/**
	@File:			stdio.c
	@Author:		Ihsoh
	@Date:			2014-1-19
	@Description:
		提供标准输入输出功能。
*/

#include "stdio.h"
#include "types.h"
#include "stdarg.h"
#include "string.h"

#include "screen.h"

extern FILE * stdin = IFS1_STDIN;
extern FILE * stdout = IFS1_STDOUT;
extern FILE * stderr = IFS1_STDERR;

int vsprintf_s(char * buffer, uint size, const char * format, va_list va)
{
	char chr;
	uint buf_len = 0;
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
					case 'u':
					{
						unsigned int u = va_arg(va, unsigned int);
						char temp[100];
						char * u_s = uitos(temp, u);
						uint u_s_len = strlen(u_s);
						if(buf_len + u_s_len >= size)
							end = 1;
						else
						{
							strncpy(buffer, u_s, u_s_len);
							buffer += u_s_len;
							buf_len += u_s_len;
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
							strncpy(buffer, d_s, d_s_len);
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
					case 's':
					{
						char * str = va_arg(va, char *);
						uint str_len = strlen(str);
						if(buf_len + str_len >= size)
							end = 1;
						else
						{
							strncpy(buffer, str, str_len);
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
	int r = vsprintf_s(buffer, 10 * 1024, format, va);
	va_end(va);
	il_print_str(buffer);
	return r;
}

int puts(const char * string)
{
	il_print_str(string);
	il_print_char('\n');
	return strlen(string);
}

int putchar(int ch)
{
	il_print_char((char)ch);
	return ch;
}

static BOOL __parse_path(const char * path, char * dir, char * file)
{
	int i = (int)(strlen(path)) - 1;
	if(i < 0)
		return FALSE;
	for(; i >= 0; i--)
		if(path[i] == '/')
			break;
	strcpy(file, path + i + 1);
	strncpy(dir, path, i + 1);
	dir[i + 1] = '\0';
	return TRUE;
}

FILE * fopen(const char * path, const char * mode)
{
	if(	path == NULL 
		|| mode == NULL
		|| strlen(path) >= 1024)
		return NULL;

	int ilmode = 0;
	if(strcmp(mode, "r") == 0)
		ilmode = FILE_MODE_READ;
	else if(strcmp(mode, "r+") == 0)
		ilmode = FILE_MODE_READ | FILE_MODE_WRITE;
	else if(strcmp(mode, "rb") == 0)
		ilmode = FILE_MODE_READ;
	else if(strcmp(mode, "rb+") == 0)
		ilmode = FILE_MODE_READ | FILE_MODE_WRITE;
	else if(strcmp(mode, "w") == 0)
		ilmode = FILE_MODE_WRITE | FILE_MODE_APPEND;
	else if(strcmp(mode, "w+") == 0)
		ilmode = FILE_MODE_READ | FILE_MODE_WRITE | FILE_MODE_APPEND;
	else if(strcmp(mode, "a") == 0)
		ilmode = FILE_MODE_APPEND;
	else if(strcmp(mode, "a+") == 0)
		ilmode = FILE_MODE_ALL;
	else if(strcmp(mode, "wb") == 0)
		ilmode = FILE_MODE_WRITE | FILE_MODE_APPEND;
	else if(strcmp(mode, "wb+") == 0)
		ilmode = FILE_MODE_READ | FILE_MODE_WRITE | FILE_MODE_APPEND;
	else if(strcmp(mode, "at") == 0)
		ilmode = FILE_MODE_APPEND;
	else if(strcmp(mode, "at+") == 0)
		ilmode = FILE_MODE_ALL;
	else
		return NULL;

	if(	strchr(mode, 'w') != NULL
		|| strchr(mode, 'a') != NULL)
		if(!ILPathExists(path))
		{
			char dir[1024];
			char file[1024];
			if(!__parse_path(path, dir, file))
				return NULL;
			if(!ILCreateFile(dir, file))
				return NULL;
		}

	ILFILE * ilfptr = ILOpenFile(path, ilmode);
	if(ilfptr == NULL)
		return NULL;
	FILE * fptr = (FILE *)malloc(sizeof(FILE));
	if(fptr == NULL)
	{
		ILCloseFile(ilfptr);
		return NULL;
	}
	fptr->ilfptr = ilfptr;
	strcpy(fptr->mode, mode);
	fptr->ilmode = ilmode;
	if(	strchr(mode, 'w') != NULL
		&& strchr(mode, 'a') == NULL)
		ILWriteFile(ilfptr, "", 0);
	fptr->old_char = EOF;
	return fptr;
}

int fclose(FILE * fptr)
{
	if(fptr == NULL)
		return EOF;
	if(ILCloseFile(fptr->ilfptr))
	{
		free(fptr);
		return 0;
	}
	else
		return EOF;
}

size_t fread(void * buffer, size_t size, size_t count, FILE * fptr)
{
	return ILReadFile(fptr->ilfptr, buffer, count * size);
}

size_t fwrite(const void * buffer, size_t size, size_t count, FILE * fptr)
{
	if(fptr == stderr || fptr == stdout)
	{
		print_str(buffer);
		return strlen(buffer);
	}
	else
	{
		uint old_size = ILGetFileLength(fptr->ilfptr);
		if(ILAppendFile(fptr->ilfptr, buffer, count * size))
			return count * size;
		else
			return ILGetFileLength(fptr->ilfptr) - old_size;
	}
}

int fgetc(FILE * fptr)
{
	if(fptr->old_char != EOF)
	{
		char c = fptr->old_char;
		fptr->old_char = EOF;
		return c;
	}
	if(ILIsEndOfFile(fptr->ilfptr))
		return EOF;
	uchar b;
	if(ILReadFile(fptr->ilfptr, &b, 1) == 0)
		return EOF;
	return (int)b;
}

int fputc(char c, FILE * fptr)
{
	if(fwrite(&c, 1, 1, fptr) == 1)
		return c;
	else
		return EOF;
}

char * fgets(char * buf, int bufsize, FILE * fptr)
{
	if(ILIsEndOfFile(fptr->ilfptr))
		return NULL;
	char * s = buf;
	int count = bufsize - 1;
	int i;
	for(i = 0; i < count; i++)
	{
		int c = fgetc(fptr);
		if(c == EOF)
			break;
		*(s++) = c;
		if(c == '\n')
			break;
	}
	*s = '\0';
	return buf;
}

int fputs(char * buf, FILE * fptr)
{
	if(fptr == stderr || fptr == stdout)
	{
		print_str(buf);
		return 0;
	}
	else
	{
		uint len = strlen(buf);
		if(fwrite(buf, len, 1, fptr) != len)
			return EOF;
		else
			return 0;
	}
}

int fprintf(FILE * fptr, const char * format, ...)
{
	char buffer[10 * 1024];
	va_list va;
	va_start(va, format);
	int r = vsprintf_s(buffer, 10 * 1024, format, va);
	va_end(va);
	if(fptr == stderr || fptr == stdout)
	{
		print_str(buffer);
		return r;
	}
	else
		if(fputs(buffer, fptr) != EOF)
			return r;
		else
			return EOF;
}

int ungetc(int c, FILE * fptr)
{
	if(fptr->old_char == EOF)
	{
		fptr->old_char = c;
		return c;
	}
	else
		return EOF;
}
