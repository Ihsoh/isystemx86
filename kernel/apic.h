/**
	@File:			apic.h
	@Author:		Ihsoh
	@Date:			2015-03-10
	@Description:
		高级可编程中断管理器。
*/

#ifndef	_APIC_H_
#define	_APIC_H_

#include "types.h"

BOOL
apic_init(void);

BOOL
apic_enable(void);

BOOL
apic_is_enable(void);

BOOL
apic_eoi(void);

#endif
