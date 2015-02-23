/**
	@File:			rsdp.h
	@Author:		Ihsoh
	@Date:			2015-02-23
	@Description:
*/

#ifndef _RSDP_H_
#define	_RSDP_H_

#include "types.h"

struct RSDPDescriptor
{
	int8 	sign[8];
	uint8 	checksum;
	int8	oemid[6];
	uint8 	revision;
	uint32 	rsdt_addr;
} __attribute__((packed));

struct RSDPDescriptor2_0
{
	struct RSDPDescriptor 	part1_0;
	uint32 					length;
	uint64 					xsdt_addr;
	uint8 					echecksum;
	uint8 					reserved[3];
} __attribute__((packed));

BOOL
rsdp_init(void);

struct RSDPDescriptor2_0 *
rsdp_get_desc(void);

#endif
