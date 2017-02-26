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
BOOL
E1000SendPacket(
	IN uint32 index,
	IN const void * packet,
	IN uint16 len);

extern
void
E1000Init(void);

#endif
