#include "lib.h"
#include "types.h"

uint32
pathl_lib_strlen(IN const int8 * str)
{
	uint32 len = 0;
	while(*(str++) != '\0')
		len++;
	return len;
}

int8 * 
pathl_lib_strcpy(	OUT int8 * dst, 
					IN const int8 * src)
{
	int8 * r = dst;
	while(*(dst++) = *(src++));
	return r;
}

int8 *
pathl_lib_strcat(	OUT int8 * dst,
					IN const int8 * src)
{
	int8 * r = dst;
	while(*dst)
		dst++;
	while(*(dst++) = *(src++));
	return r;
}

int32
pathl_lib_strcmp(	IN const int8 * str1, 
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
