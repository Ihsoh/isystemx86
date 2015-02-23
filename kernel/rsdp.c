/**
	@File:			rsdp.c
	@Author:		Ihsoh
	@Date:			2015-02-23
	@Description:
		RSDP。
*/

#include "rsdp.h"
#include "types.h"

#include <string.h>

static struct RSDPDescriptor2_0 rsdp;
static BOOL is_inited = FALSE;

BOOL
rsdp_init(void)
{
	if(is_inited)
		return TRUE;
	uint8 * ptr = 0x000e0000;
	BOOL found = FALSE;
	for(;ptr < 0x000fffff; ptr += 16)
		if(strncmp(ptr, "RSD PTR ", 8) == 0)
		{
			found = TRUE;
			break;
		}
	if(!found)
		return FALSE;
	memcpy(&rsdp, ptr, sizeof(struct RSDPDescriptor2_0));
	is_inited = TRUE;
	return TRUE;
}

struct RSDPDescriptor2_0 *
rsdp_get_desc(void)
{
	if(is_inited)
		return &rsdp;
	else
		return NULL;
}
