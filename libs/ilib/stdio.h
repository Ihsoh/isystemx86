/**
	@File:			stdio.h
	@Author:		Ihsoh
	@Date:			2014-1-19
	@Description:
*/

#ifndef	_STDIO_H_
#define	_STDIO_H_

#include "types.h"
#include "stdarg.h"
#include "fs.h"

extern int vsscanf(const char * buffer, const char * format, va_list va);
extern int sscanf(char * buffer, const char * format, ...);
extern int vsprintf_s(char * buffer, uint size, const char * format, va_list va);
extern int sprintf_s(char * buffer, uint size, const char * format, ...);
extern int sprintf(char * buffer, const char * format, ...);
extern int printf(const char * format, ...);
extern int puts(const char * string);
extern int putchar(int ch);

#define	FILE_TYPE_FILE		1
#define	FILE_TYPE_STRING	2

#define	EOF		-1

typedef struct _File
{
	// 公共
	int 			type;
	int				old_char;
	struct _File * 	redirection;

	// FILE_TYPE_FILE
	ILFILE *		ilfptr;
	int				ilmode;
	char			mode[8];

	// FILE_TYPE_STRING
	uint			str_cursor;
	uint 			str_len;
	char *			str;
} File;

#define	FILE 	File

#define IFS1_STDIN		((FILE *)0x00000010)
#define IFS1_STDOUT		((FILE *)0x00000011)
#define IFS1_STDERR		((FILE *)0x00000012)

extern FILE * stdin;
extern FILE * stdout;
extern FILE * stderr;

#define	BUFSIZ 	1024

extern FILE * fopen(const char * path,const char * mode);
extern int fclose(FILE * fptr);
extern size_t fread(void * buffer, size_t size, size_t count, FILE * fptr);
extern size_t fwrite(const void * buffer, size_t size, size_t count, FILE * fptr);
extern int fgetc(FILE * fptr);
extern int fputc(char c, FILE * fptr);
extern char * fgets(char * buf, int bufsize, FILE * fptr);
extern int fputs(char * buf, FILE * fptr);
extern int fprintf(FILE * fptr, const char * format, ...);
#define	putc(c, fptr)	(fputc((c), (fptr)))
extern int ungetc(int c, FILE * fptr);
#define	getc(fptr)	(fgetc((fptr)))
extern void perror(const char * s);
extern FILE * freopen(const char * filename, const char * mode, FILE * fptr);


extern int __vfscanf(FILE * s, const char * format, va_list arg);


extern FILE * il_sopen(const char * str);

#endif
