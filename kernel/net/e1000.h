/**
	@File:			e1000.h
	@Author:		Ihsoh
	@Date:			2017-02-25
	@Description:
		E1000 驱动。
*/

#ifndef	_NET_E1000_H_
#define	_NET_E1000_H_

#include "../types.h"

extern
uint32
E1000GetDeviceCount(void);

extern
BOOL
E1000SetMAC(
	IN uint32 index,
	IN const uint8 * mac);

extern
const uint8 *
E1000GetMAC(
	IN uint32 index);

extern
BOOL
E1000SetIP(
	IN uint32 index,
	IN const uint8 * ip);

extern
const uint8 *
E1000GetIP(
	IN uint32 index);

extern
CASCTEXT
E1000GetName(void);

extern
BOOL
E1000SendPacket(
	IN uint32 index,
	IN const void * packet,
	IN uint16 len);

extern
void
E1000Init(void);

#endif
