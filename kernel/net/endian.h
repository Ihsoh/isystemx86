/**
	@File:			endian.h
	@Author:		Ihsoh
	@Date:			2017-02-02
	@Description:
		大小端处理。
*/

#ifndef	_NET_ENDIAN_H_
#define	_NET_ENDIAN_H_

#include "../types.h"

extern
uint8 *
NetReverseByteOrder(
	IN OUT uint8 * buf,
	IN uint32 len);

extern
uint16
NetSwitchEndian16(
	IN uint16 data);

extern
uint32
NetSwitchEndian32(
	IN uint32 data);

#define	NetToBigEndian16(data) (NetSwitchEndian16((data)))
#define	NetToBigEndian32(data) (NetSwitchEndian32((data)))
#define	NetToLittleEndian16(data) (NetSwitchEndian16((data)))
#define	NetToLittleEndian32(data) (NetSwitchEndian32((data)))

#endif
