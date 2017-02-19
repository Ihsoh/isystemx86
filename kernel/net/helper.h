/**
	@File:			helper.h
	@Author:		Ihsoh
	@Date:			2017-02-19
	@Description:
		关于网络的一些工具方法。
*/

#ifndef	_NET_HELPER_H_
#define	_NET_HELPER_H_

#include "../types.h"

extern
BOOL
NetParseIP(
	IN CASCTEXT s,
	OUT uint8 * b);

extern
BOOL
NetParsePort(
	IN CASCTEXT s,
	OUT uint16 * b);

extern
BOOL
NetParseMAC(
	IN CASCTEXT s,
	OUT uint8 * b);

extern
BOOL
NetCompareIP(
	IN uint8 * a,
	IN uint8 * b);

#endif
