//Filename:		string.c
//Author:		Ihsoh
//Date:			2014-1-29
//Descriptor:	String functions

#include "string.h"
#include "types.h"

unsigned int _strlen(const char * str)
{
	unsigned int len = 0;
	while(*(str++) != '\0')
		len++;
	return len;
}

void * _memcpy(void * dst, const void * src, unsigned int count)
{
	void * r = dst;
	while(count--)
	{
		*(unsigned char *)dst = *(unsigned char *)src;
		dst = (unsigned char *)dst + 1;
		src = (unsigned char *)src + 1;
	}
	return r;
}

void * _memset(void * s, int ch, unsigned int n)
{
	unsigned char * carr = (unsigned char *)s;
	unsigned int ui;
	for(ui = 0; ui < n; ui++)
		carr[ui] = ch;
	return s;
}

char * _strcpy(char * dst, const char * src)
{
	char * r = dst;
	while(*(dst++) = *(src++));
	return r;
}

char * _strcat(char * dst, const char * src)
{
	char * r = dst;
	while(*dst)
		dst++;
	while(*(dst++) = *(src++));
	return r;
}

char * _strrev(char * str)
{
	char * r = str;
	unsigned int left = 0;
	unsigned int right = strlen(str) - 1;
	char temp;	
	while(left != right)
	{
		temp = *(str + left);
		*(str + left) = *(str + right);
		*(str + right) = temp;
		if(right - left == 1)
			break;
		left++;
		right--;
	}
	return r;
}

int _strcmp(const char * str1, const char * str2)
{
	unsigned int str1len = strlen(str1);
	unsigned int str2len = strlen(str2);
	
	if(str1len != str2len)
		return str1len - str2len;	
	while(*str1 && *str2)
		if(*str1 != *str2)
			return *str1 - *str2;
		else
		{
			str1++;
			str2++;
		}
	return 0;
}

int _strcmpn(const char * str1, const char * str2, unsigned int n)
{
	unsigned int ui;
	for(ui = 0; ui < n; ui++)
		if(str1[ui] != str2[ui])
			return str1[ui] - str2[ui];
	return 0;
}

int _memcmp(const unsigned char * str1, const unsigned char * str2, unsigned int n)
{
	unsigned int ui;
	for(ui = 0; ui < n; ui++)
		if(str1[ui] != str2[ui])
			return str1[ui] - str2[ui];
	return 0;
}

char * _strchr(const char * s, char c)
{
	while(*s != '\0' && *s != c)
		++s;
	return *s == c ? s : NULL;
}

char * _strstr(const char * s1, const char * s2)
{
	int n;
	if (*s2)
	{
		while (*s1)
		{
			for (n=0; *(s1 + n) == *(s2 + n); n++)
			{
				if (!*(s2 + n + 1))
					return (char *)s1;
			}
			s1++;
		}
		return NULL;
	}
	else
		return (char *)s1;
}





char * ultos(char * str, unsigned long n)
{
	char * r = str;
	unsigned int index = 0;
	if(n == 0)
		*(str + index++) = '0';
	else
		while(n != 0)
		{
			char chr = n - n / 10 * 10;
			str[index++] = chr + '0';
			n /= 10;
		}
	str[index] = '\0';
	strrev(str);
	return r;
}

char * dtos(char * str, double n)
{
	char * s = str;
	if(n < 0.0)
	{
		n *= -1.0;
		*s = '-';
		s++;
	}
	else if(n == 0.0)
	{
		strcpy(str, "0");
		return str;
	}
	unsigned int i, d;
	i = (unsigned int)n;
	d = (unsigned int)((n - i + 1) * 1000000000.0);
	uitos(s, i);
	unsigned int len = strlen(s);
	s[len] = '.';
	uitos(s + len + 1, d);
	s[len + 1] = ' ';
	ltrim(s + len + 1);
	rtrimex(str, '0');
	len = strlen(s);
	if(str[len - 1] == '.')
		str[len - 1] = '\0';
	return str;
}

char * uctohexs(char * str, unsigned char n)
{
	unsigned char temp;
	temp = 0xf & (n >> 4);
	*str = temp < 0xa ? temp + '0' : temp - 0xa + 'A';
	temp = 0xf & n;
	*(str + 1) = temp < 0xa ? temp + '0' : temp - 0xa + 'A';
	*(str + 2) = '\0';
	return str;
}

char * ustohexs(char * str, unsigned short n)
{
	uctohexs(str, (unsigned char)(n >> 8));
	uctohexs(str + 2, (unsigned char)n);
	return str;
}

char * uitohexs(char * str, unsigned int n)
{
	uctohexs(str, (unsigned char)(n >> 24));
	uctohexs(str + 2, (unsigned char)(n >> 16));
	uctohexs(str + 4, (unsigned char)(n >> 8));
	uctohexs(str + 6, (unsigned char)n);
	return str;
}

char * ltos(char * str, long n)
{
	char * r = str;
	if(n < 0)
	{
		*(str++) = '-';
		n *= -1;
	}	
	ultos(str, n);
	return r;
}

unsigned long stoul(char * str)
{
	int i = strlen(str) - 1;
	unsigned long n = 0;
	unsigned long m = 1;
	for(;i >= 0; i--)
	{
		n += m * (*(str + i) - '0');
		m *= 10;
	}
	return n;
}

long stol(char * str)
{
	if(*str == '-')
		return stoul(str + 1) * -1;
	else if(*str == '+')
		return stoul(str + 1);
	else
		return stoul(str);
}

double stod(char * str)
{
	char temp[1024];
	unsigned int len;
	if((len = strlen(str)) >= sizeof(temp))
		return 0.0;
	strcpy(temp, str);
	int comma;
	for(comma = 0; comma < len; comma++)
		if(temp[comma] == '.')
			break;
	double i = 0;
	double d = 0;
	unsigned int dp = 0;
	double n = 0.0;
	if(comma < len)
	{
		temp[comma] = '\0';
		i = (double)stol(temp);
		d = (double)stoul(temp + comma + 1);
		dp = strlen(temp + comma + 1);
		while(dp--)
			d /= 10.0;
		n = i + d;
	}
	else
		n = (double)stol(str);
	return n;
}

void ltrim(char * str)
{
	unsigned int start;	
	unsigned int ui;
	unsigned int len = strlen(str);
	for(start = 0; start < len && str[start] == ' '; start++);
	len = len - start + 1;
	for(ui = 0; ui < len; ui++, start++)
		str[ui] = str[start];
}

void rtrim(char * str)
{
	int start;
	for(start = strlen(str) - 1; start >= 0 && str[start] == ' '; start--);
	str[start + 1] = '\0';
}

void trim(char * str)
{
	ltrim(str);
	rtrim(str);
}

void ltrimex(char * str, char chr)
{
	unsigned int start;	
	unsigned int ui;
	unsigned int len = strlen(str);
	for(start = 0; start < len && str[start] == chr; start++);
	len = len - start + 1;
	for(ui = 0; ui < len; ui++, start++)
		str[ui] = str[start];
}

void rtrimex(char * str, char chr)
{
	int start;
	for(start = strlen(str) - 1; start >= 0 && str[start] == chr; start--);
	str[start + 1] = '\0';
}

void trimex(char * str, char chr)
{
	ltrimex(str, chr);
	rtrimex(str, chr);
}

int is_valid_uint(char * str)
{
	char chr;
	while((chr = *(str++)) != '\0')
		if(!(chr >= '0' && chr <= '9'))
			return 0;
	return 1;
}
