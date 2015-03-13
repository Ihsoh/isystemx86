/**
	@File:			madt.h
	@Author:		Ihsoh
	@Date:			2015-03-10
	@Description:
		MADT。
*/

#ifndef	_MADT_H_
#define	_MADT_H_

#include "types.h"
#include "rsdt.h"

struct MADTEntryHeader
{
	uint8	type;
	uint8	length;
} __attribute__((packed));

//Processor Local APIC
struct MADTEntryType0
{
	uint8	acpi_processor_id;
	uint8	apic_id;
	uint32	flags;
} __attribute__((packed));

//I/O APIC
struct MADTEntryType1
{
	uint8	ioapic_id;
	uint8	reserved;
	uint32	ioapic_address;
	uint32	global_sys_int_base;
} __attribute__((packed));

//Interrupt Source Override
struct MADTEntryType2
{
	uint8	bus_source;
	uint8	irq_source;
	uint32	global_sys_int;
	uint16	flags;
} __attribute__((packed));

struct MADT
{
	struct ACPISDTHeader	header;
	uint32					lcaddr;
	uint32					flags;
	struct MADTEntryHeader	first_entry_header;
} __attribute__((packed));

extern
BOOL
madt_init(void);

extern
uint32
madt_get_type0_count(void);

extern
uint32
madt_get_type1_count(void);

extern
uint32
madt_get_type2_count(void);

extern
struct MADTEntryType0 *
madt_get_type0_entry(IN uint32 index);

extern
struct MADTEntryType1 *
madt_get_type1_entry(IN uint32 index);

extern
struct MADTEntryType2 *
madt_get_type2_entry(IN uint32 index);

extern
struct MADT *
madt_get(void);

extern
BOOL
madt_write_to_file(IN const int8 * path);

#endif
