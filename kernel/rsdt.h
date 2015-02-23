/**
	@File:			rsdt.h
	@Author:		Ihsoh
	@Date:			2015-02-23
	@Description:
		RSDT。
*/

#ifndef	_RSDT_H_
#define	_RSDT_H_

#include "types.h"

struct ACPISDTHeader
{
	int8		sign[4];
	uint32 		length;
	uint8 		revision;
	uint8 		checksum;
	int8		oem_id[6];
	int8		oem_tableid[8];
	uint32 		oem_revision;
	uint32 		creator_id;
	uint32 		creator_revision;
} __attribute__((packed));

struct RSDT
{
	struct ACPISDTHeader header;
	uint32 first_sdt;
} __attribute__((packed));

BOOL
rsdt_init(void);

void
rsdt_free(void);

struct RSDT *
rsdt_get_table(void);

struct ACPISDTHeader *
rsdt_find_sdt(int8 * sign);

#endif
