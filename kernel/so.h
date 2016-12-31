/**
	@File:			so.h
	@Author:		Ihsoh
	@Date:			2017-01-01
	@Description:
		内核加载Elf Shared Object的功能。
*/

#ifndef	_SO_H_
#define	_SO_H_

#include "types.h"

extern
void
KnlInitElfSharedObject(void);

extern
uint32
KnlLoadElfSharedObjectFile(
	IN CASCTEXT path);

extern
void *
KnlGetElfSharedObjectSymbol(
	IN uint32 ctx_idx,
	IN CASCTEXT name);

extern
BOOL
KnlUnloadElfSharedObjectFile(
	IN uint32 ctx_idx);

#endif
