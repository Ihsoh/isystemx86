/**
	@File:			helper.c
	@Author:		Ihsoh
	@Date:			2017-02-19
	@Description:
		关于网络的一些工具方法。
*/

#include "helper.h"

/**
	@Function:		NetParseIP
	@Access:		Public
	@Description:
		解析IP地址字符串（aaa.bbb.ccc.ddd），每段使用十进制表示，范围为0~255。
	@Parameters:
		s, CASCTEXT, IN
			IP地址字符串。
		b, uint8 *, OUT
			储存IP地址的缓冲区，该缓冲区的长度至少有4个字节。
	@Return:
		BOOL
			返回TRUE则解析成功，否则失败。	
*/
BOOL
NetParseIP(
	IN CASCTEXT s,
	OUT uint8 * b)
{
	uint32 i, j;
	uint32 n;

	if (b == NULL)
	{
		goto err;
	}

	// aaa.bbb.ccc.
	for (j = 0; j < 3; j++)
	{
		n = 0;
		for (	i = 0;
				i < 3 && *s >= '0' && *s <= '9';
				i++, s++)
		{
			n *= 10;
			n += *s - '0';
		}
		if (i == 0 || *(s++) != '.' || n > 255)
		{
			goto err;
		}
		b[j] = (uint8)n;
	}

	// ddd
	n = 0;
	for (i = 0; i < 3 && *s >= '0' && *s <= '9'; i++, s++)
	{
		n *= 10;
		n += *s - '0';
	}
	if (i == 0 || *s != '\0' || n > 255)
	{
		goto err;
	}
	b[3] = (uint8)n;

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		NetParsePort
	@Access:		Public
	@Description:
		解析端口字符串，端口数值范围为0~65535。
	@Parameters:
		s, CASCTEXT, IN
			端口字符串。
		b, uint16 *, OUT
			储存端口的缓冲区。
	@Return:
		BOOL
			返回TRUE则解析成功，否则失败。	
*/
BOOL
NetParsePort(
	IN CASCTEXT s,
	OUT uint16 * b)
{
	uint32 i;
	uint32 n;

	if (b == NULL)
	{
		goto err;
	}

	n = 0;
	for (i = 0; i < 5 && *s >= '0' && *s <= '9'; i++, s++)
	{
		n *= 10;
		n += *s - '0';
	}
	if (i == 0 || *s != '\0' || n > 65535)
	{
		goto err;
	}
	*b = (uint16)n;

	return TRUE;
err:
	return FALSE;
}

static
BOOL
_NetIsHex(
	IN CASCCHAR c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static
uint8
_NetHex(
	IN CASCCHAR c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	else if(c >= 'a' && c <= 'f')
	{
		return 10 + (c - 'a');
	}
	else if(c >= 'A' && c <= 'F')
	{
		return 10 + (c - 'A');
	}
	else
	{
		return 0;
	}
}

/**
	@Function:		NetParseMAC
	@Access:		Public
	@Description:
		解析MAC地址字符串（aa:bb:cc:dd:ee:ff），每段使用十六进制表示，范围为0x00~0xff。
	@Parameters:
		s, CASCTEXT, IN
			MAC地址字符串。
		b, uint8 *, OUT
			储存MAC地址的缓冲区，该缓冲区的长度至少有6个字节。
	@Return:
		BOOL
			返回TRUE则解析成功，否则失败。	
*/
BOOL
NetParseMAC(
	IN CASCTEXT s,
	OUT uint8 * b)
{
	uint32 i, j;
	uint32 n;

	if (b == NULL)
	{
		goto err;
	}

	// aa:bb:cc:dd:ee:
	for (j = 0; j < 5; j++)
	{
		n = 0;
		for (	i = 0;
				i < 2 && _NetIsHex(*s);
				i++, s++)
		{
			n *= 0x10;
			n += _NetHex(*s);
		}
		if (i == 0 || *(s++) != ':' || n > 255)
		{
			goto err;
		}
		b[j] = (uint8)n;
	}

	// ff
	n = 0;
	for (i = 0; i < 2 && _NetIsHex(*s); i++, s++)
	{
		n *= 10;
		n += _NetHex(*s);
	}
	if (i == 0 || *s != '\0' || n > 255)
	{
		goto err;
	}
	b[5] = (uint8)n;

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		NetCompareIP
	@Access:		Public
	@Description:
		比较两个IP地址是否相等。
	@Parameters:
		a, uint8 *, IN
			IP地址。
		b, uint8 *, IN
			IP地址。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
NetCompareIP(
	IN uint8 * a,
	IN uint8 * b)
{
	return *(uint32 *)a == *(uint32 *)b;
}

/**
	@Function:		NetCompareMAC
	@Access:		Public
	@Description:
		比较两个MAC地址是否相等。
	@Parameters:
		a, uint8 *, IN
			MAC地址。
		b, uint8 *, IN
			MAC地址。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
NetCompareMAC(
	IN uint8 * a,
	IN uint8 * b)
{
	return 	*(uint32 *)a == *(uint32 *)b
			&& a[4] == b[4]
			&& a[5] == b[5];
}
