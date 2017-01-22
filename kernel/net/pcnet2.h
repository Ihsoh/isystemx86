/**
	@File:			pcnet2.h
	@Author:		Ihsoh
	@Date:			2017-01-23
	@Description:
		PCnet-PCI II 驱动。
*/

#ifndef	_NET_PCNET2_H_
#define	_NET_PCNET2_H_

#include "../types.h"

extern
void
PCNET2Init(void);

extern
uint32
PCNET2GetDeviceCount(void);

#endif
