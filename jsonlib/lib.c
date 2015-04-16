#include "lib.h"
#include "types.h"

void *
jsonl_lib_memcpy(	OUT void * dst,
					IN const void * src,
					IN uint32 count)
{
	void * r = dst;
	while(count--)
	{
		*(uint8 *)dst = *(uint8 *)src;
		dst = (uint8 *)dst + 1;
		src = (uint8 *)src + 1;
	}
	return r;
}

uint32
jsonl_lib_strlen(IN const int8 * str)
{
	uint32 len = 0;
	while(*(str++) != '\0')
		len++;
	return len;
}

int32
jsonl_lib_strcmp(	IN const int8 * str1, 
					IN const int8 * str2)
{
	uint32 str1len = jsonl_lib_strlen(str1);
	uint32 str2len = jsonl_lib_strlen(str2);
	
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

int8 * 
jsonl_lib_strcpy(	OUT int8 * dst, 
					IN const int8 * src)
{
	int8 * r = dst;
	while(*(dst++) = *(src++));
	return r;
}

uint32
jsonl_lib_stoul(IN int8 * str)
{
	int32 i = jsonl_lib_strlen(str) - 1;
	uint32 n = 0;
	uint32 m = 1;
	for(;i >= 0; i--)
	{
		n += m * (*(str + i) - '0');
		m *= 10;
	}
	return n;
}

int32
jsonl_lib_stol(IN int8 * str)
{
	if(*str == '-')
		return jsonl_lib_stoul(str + 1) * -1;
	else if(*str == '+')
		return jsonl_lib_stoul(str + 1);
	else
		return jsonl_lib_stoul(str);
}

double
jsonl_lib_stod(IN int8 * str)
{
	int8 temp[1024];
	uint32 len;
	if((len = jsonl_lib_strlen(str)) >= sizeof(temp))
		return 0.0;
	jsonl_lib_strcpy(temp, str);
	int32 comma;
	for(comma = 0; comma < len; comma++)
		if(temp[comma] == '.')
			break;
	double i = 0;
	double d = 0;
	uint32 dp = 0;
	double n = 0.0;
	if(comma < len)
	{
		temp[comma] = '\0';
		i = (double)jsonl_lib_stol(temp);
		d = (double)jsonl_lib_stoul(temp + comma + 1);
		dp = jsonl_lib_strlen(temp + comma + 1);
		while(dp--)
			d /= 10.0;
		n = i + d;
	}
	else
		n = (double)jsonl_lib_stol(str);
}
