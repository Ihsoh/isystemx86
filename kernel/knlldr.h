/**
	@File:			knlldr.h
	@Author:		Ihsoh
	@Date:			2015-05-18
	@Description:
		与内核加载器相关的信息。
*/

#ifndef	_KNLLDR_H_
#define	_KNLLDR_H_

#include "types.h"

#define	KNLLDR_HEADER_KERNEL_TSS		0
#define	KNLLDR_HEADER_MEMORY_SIZE_AX	1
#define	KNLLDR_HEADER_MEMORY_SIZE_BX	2
#define	KNLLDR_HEADER_VESA_ENABLED		3
#define	KNLLDR_HEADER_VESA_INFO			4
#define	KNLLDR_HEADER_VESA_MODE			5
#define	KNLLDR_HEADER_GDT				6
#define	KNLLDR_HEADER_IDT				7
#define	KNLLDR_HEADER_VGDTR				8
#define	KNLLDR_HEADER_VIDTR				9

extern uint32 * knlldr_header;

#define	KNLLDR_HEADER_ITEM(__index)	((void *)(knlldr_header[(__index)]))

BOOL
knlldr_init(void);

#endif
