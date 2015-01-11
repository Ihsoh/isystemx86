/**
	@File:			kfuncs.h
	@Author:		Ihsoh
	@Date:			2014-7-3
	@Description:
*/

#ifndef	_KFUNCS_H_
#define	_KFUNCS_H_

#include "types.h"

extern
uint64
get_memory_size(void);

extern
uint32
get_gdt_addr(void);

extern
uint32
get_idt_addr(void);

extern
uint32
get_vesa_info_addr(OUT uint32 * mode);

extern
uint32
get_kernel_tss_addr(void);

#endif
