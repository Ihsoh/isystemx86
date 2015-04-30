/**
	@File:			rsdp.c
	@Author:		Ihsoh
	@Date:			2015-02-23
	@Description:
		RSDP。
*/

#include "rsdp.h"
#include "types.h"

#include <ilib/string.h>

static struct RSDPDescriptor2_0 rsdp;
static BOOL is_inited = FALSE;

/**
	@Function:		rsdp_init
	@Access:		Public
	@Description:
		初始化 RSDP。
	@Parameters:
	@Return:
		BOOL
			初始化成功则返回 TRUE，否则返回 FALSE。
*/
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

/**
	@Function:		rsdp_get_desc
	@Access:		Public
	@Description:
		获取 RSDP。
	@Parameters:
	@Return:
		struct RSDPDescriptor2_0 *
			成功返回 RSDP，否则返回 NULL。		
*/
struct RSDPDescriptor2_0 *
rsdp_get_desc(void)
{
	if(is_inited)
		return &rsdp;
	else
		return NULL;
}
