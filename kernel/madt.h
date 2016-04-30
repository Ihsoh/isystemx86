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
MadtInit(void);

extern
uint32
MadtGetType0Count(void);

extern
uint32
MadtGetType1Count(void);

extern
uint32
MadtGetType2Count(void);

extern
struct MADTEntryType0 *
MadtGetType0Entry(IN uint32 index);

extern
struct MADTEntryType1 *
MadtGetType1Entry(IN uint32 index);

extern
struct MADTEntryType2 *
MadtGetType2Entry(IN uint32 index);

extern
struct MADT *
MadtGet(void);

extern
BOOL
MadtWriteToFile(IN const int8 * path);

#endif
