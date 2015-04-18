/**
	@File:			acpi.h
	@Author:		Ihsoh
	@Date:			2015-02-23
	@Description:
*/

#ifndef _ACPI_H_
#define	_ACPI_H_

#include "types.h"

BOOL
acpi_init(void);

BOOL
acpi_power_off(void);

BOOL
apic_stop_timer(void);

BOOL
apic_start_timer(void);

#endif
