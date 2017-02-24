/**
	@File:			pcnet2.h
	@Author:		Ihsoh
	@Date:			2017-01-23
	@Description:
		AMD PCnet-PCI II 驱动。
*/

#ifndef	_NET_PCNET2_H_
#define	_NET_PCNET2_H_

#include "../types.h"

extern
uint32
PCNET2GetDeviceCount(void);

extern
BOOL
PCNET2SetMAC(
	IN uint32 index,
	IN const uint8 * mac);

extern
const uint8 *
PCNET2GetMAC(
	IN uint32 index);

extern
BOOL
PCNET2SetIP(
	IN uint32 index,
	IN const uint8 * ip);

extern
const uint8 *
PCNET2GetIP(
	IN uint32 index);

extern
CASCTEXT
PCNET2GetName(void);

extern
BOOL
PCNET2SendPacket(
	IN uint32 index,
	IN const void * packet,
	IN uint16 len);

extern
void
PCNET2PrintStatus(
	IN uint32 index);

extern
void
PCNET2Init(void);

#endif
