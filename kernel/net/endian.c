/**
	@File:			endian.c
	@Author:		Ihsoh
	@Date:			2017-02-02
	@Description:
		大小端处理。
*/

#include "endian.h"

uint8 *
NetReverseByteOrder(
	IN OUT uint8 * buf,
	IN uint32 len)
{
	uint32 m = len / 2;
	uint32 left, right;
	for (left = 0, right = len - 1; left < m; left++, right--)
	{
		uint32 temp = buf[right];
		buf[right] = buf[left];
		buf[left] = temp;
	}
	return buf;
}

uint16
NetSwitchEndian16(
	IN uint16 data)
{
	NetReverseByteOrder(&data, sizeof(data));
	return data;
}

uint32
NetSwitchEndian32(
	IN uint32 data)
{
	NetReverseByteOrder(&data, sizeof(data));
	return data;
}
