/**
	@File:			sfstr.c
	@Author:		Ihsoh
	@Date:			2015-11-14
	@Description:
		安全的字符串操作函数库。
*/

#include "sfstr.h"

#include "../types.h"

/**
	@Function:		UtlGetStringLength
	@Access:		Public
	@Description:
		安全地获取字符串的长度。
	@Parameters:
		len, uint32 *, OUT
			指向储存字符串长度的缓冲区的指针。
		src, CASCTEXT, IN
			字符串。
	@Return:
		SFSTR_RESULT
			字符串操作结果。
*/
SFSTR_RESULT
UtlGetStringLength(	OUT uint32 * len,
					IN CASCTEXT str)
{
	if(len == NULL || str == NULL)
		return SFSTR_R_NULL;
	*len = 0;
	while(*(str++) != '\0')
		(*len)++;
	return SFSTR_R_OK;
}

/**
	@Function:		UtlCompareString
	@Access:		Public
	@Description:
		安全地比较两个字符串。
	@Parameters:
		str0, CASCTEXT, IN
			需要进行比较的字符串。
		str1, CASCTEXT, IN
			需要进行比较的字符串。
	@Return:
		SFSTR_RESULT
			如果两个字符串相等，则返回SFSTR_R_EQUAL。
			如果两个字符串不相等，则返回SFSTR_R_NOT_EQUAL。
*/
SFSTR_RESULT
UtlCompareString(	IN CASCTEXT str0,
					IN CASCTEXT str1)
{
	if(str0 == NULL || str1 == NULL)
		return SFSTR_R_NULL;
	uint32 str0len = 0;
	uint32 str1len = 0;
	SFSTR_RESULT r;
	if((r = UtlGetStringLength(&str0len, str0)) != SFSTR_R_OK)
		return r; 
	if((r = UtlGetStringLength(&str1len, str1)) != SFSTR_R_OK)
		return r; 
	if(str0len != str1len)
		return SFSTR_R_NOT_EQUAL;
	while(*str0 && *str1)
		if(*str0 != *str1)
			return SFSTR_R_NOT_EQUAL;
		else
		{
			str0++;
			str1++;
		}
	return SFSTR_R_EQUAL;
}

/**
	@Function:		UtlCopyString
	@Access:		Public
	@Description:
		安全地复制字符串。
	@Parameters:
		dst, ASCTEXT, OUT
			目标字符串缓冲区。
		size, uint32, IN
			目标字符串缓冲区大小。
		src, CASCTEXT, IN
			源字符串缓冲区。
	@Return:
		SFSTR_RESULT
			字符串操作结果。
*/
SFSTR_RESULT
UtlCopyString(	OUT ASCTEXT dst,
				IN uint32 size,
				IN CASCTEXT src)
{
	if(dst == NULL || src == NULL)
		return SFSTR_R_NULL;
	if(size == 0)
		return SFSTR_R_TOO_LONG;
	uint32 ui;
	for(ui = 0; ui < size - 1 && src[ui] != '\0'; ui++)
		dst[ui] = src[ui];
	dst[ui] = '\0';
	if(src[ui] != '\0')
		return SFSTR_R_TOO_LONG;
	else
		return SFSTR_R_OK;
}

/**
	@Function:		UtlConcatString
	@Access:		Public
	@Description:
		安全地把一个字符串追加到另一个字符串的结尾。
	@Parameters:
		dst, ASCTEXT, IN OUT
			目标字符串缓冲区。
		size, uint32, IN
			目标字符串缓冲区大小。
		src, CASCTEXT, IN
			源字符串缓冲区。
	@Return:
		SFSTR_RESULT
			字符串操作结果。
*/
SFSTR_RESULT
UtlConcatString(IN OUT ASCTEXT dst,
				IN uint32 size,
				IN CASCTEXT src)
{
	uint32 dst_len;
	SFSTR_RESULT r = UtlGetStringLength(&dst_len, dst);
	if(r == SFSTR_R_OK)
	{
		uint32 remain = size - dst_len;
		return UtlCopyString(dst + dst_len, remain, src);
	}
	else
		return r;
}

/**
	@Function:		UtlCopyMemory
	@Access:		Public
	@Description:
		安全地把一个缓冲区的数据复制到另一个缓冲区。
	@Parameters:
		dst, void *, OUT
			目标缓冲区。
		size, uint32, IN
			目标缓冲区大小。
		src, void *, IN
			源缓冲区。
		n, uint32, IN
			需要复制的字节数。
	@Return:
		SFSTR_RESULT
			字符串操作结果。
*/
SFSTR_RESULT
UtlCopyMemory(	OUT void * dst,
				IN uint32 size,
				IN void * src,
				IN uint32 n)
{
	uint8 * d = (uint8 *)dst;
	const uint8 * s = (const uint8 *)src;
	if(d == NULL || s == NULL)
		return SFSTR_R_NULL;
	uint32 ui;
	for(ui = 0; ui < n; ui++)
	{
		if(ui > size)
			return SFSTR_R_TOO_LONG;
		d[ui] = s[ui];
	}
	return SFSTR_R_OK;
}

#ifdef _KERNEL_DEBUG_

#include "../test.h"

TEST_CONTEXT(UtlSfstr)

TEST_FUNC_BEGIN(UtlGetStringLength)
	CASCTEXT str = "ABC";
	uint32 len = 0;
	TEST_BEGIN(0)
		if(UtlGetStringLength(NULL, NULL) != SFSTR_R_NULL)
			TEST_FAILED();
	TEST_END
	TEST_BEGIN(1)
		if(UtlGetStringLength(&len, str) == SFSTR_R_OK)
		{
			if(len != 3)
				TEST_FAILED();
		}
		else
			TEST_FAILED();
	TEST_END
TEST_FUNC_END

TEST_FUNC_BEGIN(UtlCompareString)
	TEST_BEGIN(0)
		if(UtlCompareString(NULL, NULL) != SFSTR_R_NULL)
			TEST_FAILED();
	TEST_END
	TEST_BEGIN(1)
		if(UtlCompareString("ABC", "ABC") != SFSTR_R_EQUAL)
			TEST_FAILED();
	TEST_END
	TEST_BEGIN(2)
		if(UtlCompareString("ABCD", "ABC") != SFSTR_R_NOT_EQUAL)
			TEST_FAILED();
	TEST_END
TEST_FUNC_END

TEST_FUNC_BEGIN(UtlCopyString)
	ASCCHAR buffer[4];
	TEST_BEGIN(0)
		if(UtlCopyString(NULL, 0, NULL) != SFSTR_R_NULL)
			TEST_FAILED();
	TEST_END
	TEST_BEGIN(1)
		if(UtlCopyString(buffer, sizeof(buffer), "ABC") == SFSTR_R_OK)
		{
			if(UtlCompareString(buffer, "ABC") != SFSTR_R_EQUAL)
				TEST_FAILED();
		}
		else
			TEST_FAILED();
	TEST_END
	TEST_BEGIN(2)
		if(UtlCopyString(buffer, sizeof(buffer), "ABCD") == SFSTR_R_TOO_LONG)
		{
			if(UtlCompareString(buffer, "ABC") != SFSTR_R_EQUAL)
				TEST_FAILED();
		}
		else
			TEST_FAILED();
	TEST_END
TEST_FUNC_END

TEST_FUNC_BEGIN(UtlConcatString)
	ASCCHAR buffer[4];
	buffer[0] = '\0';
	TEST_BEGIN(0)
		if(UtlConcatString(NULL, 0, NULL) != SFSTR_R_NULL)
			TEST_FAILED();
	TEST_END
	TEST_BEGIN(1)
		if(UtlConcatString(buffer, sizeof(buffer), "AB") != SFSTR_R_OK)
			TEST_FAILED();
	TEST_END
	TEST_BEGIN(2)
		if(UtlConcatString(buffer, sizeof(buffer), "CD") != SFSTR_R_TOO_LONG)
			TEST_FAILED();
	TEST_END
	TEST_BEGIN(3)
		if(UtlCompareString(buffer, "ABC") != SFSTR_R_EQUAL)
			TEST_FAILED();
	TEST_END
TEST_FUNC_END

TEST_FUNC_BEGIN(UtlCopyMemory)
	ASCCHAR buffer[4];
	TEST_BEGIN(0)
		if(UtlCopyMemory(NULL, 0, NULL, 0) != SFSTR_R_NULL)
			TEST_FAILED();
	TEST_END
	TEST_BEGIN(1)
		if(UtlCopyMemory(buffer, sizeof(buffer), "ABC", sizeof("ABC")) != SFSTR_R_OK)
			TEST_FAILED();
		if(UtlCompareString(buffer, "ABC") != SFSTR_R_EQUAL)
			TEST_FAILED();
	TEST_END
	TEST_BEGIN(2)
		if(UtlCopyMemory(buffer, sizeof(buffer), "ABC\0DEF", sizeof("ABC\0DEF")) != SFSTR_R_TOO_LONG)
			TEST_FAILED();
		if(UtlCompareString(buffer, "ABC") != SFSTR_R_EQUAL)
			TEST_FAILED();
	TEST_END
TEST_FUNC_END

BOOL
UtlSfstrTest(void)
{
	return 	TEST(UtlGetStringLength)
			&& TEST(UtlCompareString)
			&& TEST(UtlCopyString)
			&& TEST(UtlConcatString)
			&& TEST(UtlCopyMemory);
}

#endif
