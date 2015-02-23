/**
	@File:			fadt.h
	@Author:		Ihsoh
	@Date:			2015-02-24
	@Description:
*/

#ifndef	_FADT_H_
#define	_FADT_H_

#include "types.h"
#include "rsdt.h"

struct GenericAddressStructure
{
	uint8	address_space;
	uint8	bit_width;
	uint8	bit_offset;
	uint8	access_size;
	uint64	address;
} __attribute__((packed));

struct FADT
{
	struct ACPISDTHeader	header;
	uint32					fireware_ctrl;
	uint32					dsdt;

	uint8					reserved;

	uint8					preferred_power_management_profile;
	uint16					sci_interrupt;
	uint32					smi_command_port;
	uint8					acpi_enable;
	uint8					acpi_disable;
	uint8					s4bios_req;
	uint8					pstate_control;
	uint32					pm1a_event_block;
	uint32					pm1b_event_block;
	uint32					pm1a_control_block;
	uint32					pm1b_control_block;
	uint32					pm2_control_block;
	uint32					pm_timer_block;
	uint32					gpe0_block;
	uint32					gpe1_block;
	uint8					pm1_event_length;
	uint8					pm1_control_length;
	uint8					pm2_control_length;
	uint8					pm_timer_length;
	uint8					gpe0_length;
	uint8					gpe1_length;
	uint8					gpe1_base;
	uint8					cstate_control;
	uint16					worst_c2_latency;
	uint16					worst_c3_latency;
	uint16					flush_size;
	uint16					flush_stride;
	uint8					duty_offset;
	uint8					duty_width;
	uint8					day_alarm;
	uint8					month_alarm;
	uint8					century;

	uint16					boot_architecture_flags;

	uint8					reserved2;
	uint32					flags;

	struct GenericAddressStructure	reset_reg;

	uint8					reset_value;
	uint8					reserved3;

	uint64					x_fireware_control;
	uint64					x_dsdt;

	struct GenericAddressStructure	x_pm1a_event_block;
	struct GenericAddressStructure	x_pm1b_event_block;
	struct GenericAddressStructure	x_pm1a_control_block;
	struct GenericAddressStructure	x_pm1b_control_block;
	struct GenericAddressStructure	x_pm2_control_block;
	struct GenericAddressStructure	x_pm_timer_block;
	struct GenericAddressStructure	x_gpe0_block;
	struct GenericAddressStructure	x_gpe1_block;
};

BOOL
fadt_init(void);

struct FADT *
fadt_get_table(void);

#endif
