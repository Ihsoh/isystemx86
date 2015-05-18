/**
	@File:			knlldr.c
	@Author:		Ihsoh
	@Date:			2015-05-18
	@Description:
		与内核加载器相关的信息。
*/

#include "knlldr.h"
#include "types.h"

uint32 * knlldr_header = (uint32 *)0x10000;

BOOL
knlldr_init(void)
{
	return TRUE;
}
