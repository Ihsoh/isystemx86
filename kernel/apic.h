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
ApicInit(void);

BOOL
ApicEnable(void);

BOOL
ApicIsEnabled(void);

BOOL
ApicStopTimer(void);

BOOL
ApicStartTimer(void);

BOOL
ApicEOI(void);

#endif
