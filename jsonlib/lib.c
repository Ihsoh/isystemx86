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
	uint32 str1len = dsl_lib_strlen(str1);
	uint32 str2len = dsl_lib_strlen(str2);
	
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
