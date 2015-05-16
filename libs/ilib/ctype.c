#include "ctype.h"

int isalpha(int ch)
{
	if(	(ch >= 'a' && ch <= 'z')
		|| (ch >= 'A' && ch <= 'Z'))
		return 1;
	return 0;
}

int isalnum(int ch)
{
	if(	(ch >= '0' && ch <= '9')
		|| (ch >= 'a' && ch <= 'z')
		|| (ch >= 'A' && ch <= 'Z'))
		return 1;
	return 0;
}

int isascii(int ch)
{
	if(ch >= 0 && ch <= 127)
		return 1;
	return 0;
}

int iscntrl(int ch)
{
	if(	ch == 0x7f
		|| (ch >= 0x00 && ch <= 0x1f))
		return 1;
	return 0;
}

int isdigit(int ch)
{
	if(ch >= '0' && ch <= '9')
		return 1;
	return 0;
}

int isgraph(int ch)
{
	if(ch >= 0x21 && ch <= 0x7e)
		return 1;
	return 0;
}

int islower(int ch)
{
	if(ch >= 'a' && ch <= 'z')
		return 1;
	return 0;
}

int isprint(int ch)
{
	if(ch >= 0x20 && ch <= 0x7e)
		return 1;
	return 0;
}

int ispunct(int ch)
{
	if(ch >= 0x00 && ch <= 0x1f)
		return 1;
	return 0;
}

int isspace(int ch)
{
	if(	ch == ' '
		|| ch == '\t'
		|| ch == '\r'
		|| ch == '\f'
		|| ch == '\v'
		|| ch == '\n')
		return 1;
	return 0;
}

int isupper(int ch)
{
	if(ch >= 'A' && ch <= 'Z')
		return 1;
	return 0;
}

int isxdigit(int ch)
{
	if(	(ch >= '0' && ch <= '9')
		|| (ch >= 'a' && ch <= 'f')
		|| (ch >= 'A' && ch <= 'F'))
		return 1;
	return 0;
}

int tolower(int ch)
{
	if(ch >= 'A' && ch <= 'Z')
		return 'a' + (ch - 'A');
	return ch;
}

int toupper(int ch)
{
	if(ch >= 'a' && ch <= 'z')
		return 'A' + (ch - 'a');
	return ch;
}
