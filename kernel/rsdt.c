/**
	@File:			rsdt.c
	@Author:		Ihsoh
	@Date:			2015-02-23
	@Description:
		RSDT。
*/

#include "rsdt.h"
#include "types.h"
#include "rsdp.h"
#include "memory.h"

#include <string.h>

static struct RSDT * rsdt = NULL;

/**
	@Function:		rsdt_init
	@Access:		Public
	@Description:
		初始化 RSDT。
	@Parameters:
	@Return:
		BOOL
			初始化成功则返回 TRUE，否则返回 FALSE。
*/
BOOL
rsdt_init(void)
{
	if(rsdt != NULL)
		return TRUE;
	if(!rsdp_init())
		return FALSE;
	struct RSDPDescriptor2_0 * desc = rsdp_get_desc();
	if(desc == NULL)
		return FALSE;
	struct ACPISDTHeader * header = (struct ACPISDTHeader *)desc->part1_0.rsdt_addr;
	if(header == NULL)
		return FALSE;
	rsdt = alloc_memory(header->length);
	memcpy(rsdt, header, header->length);
	return TRUE;
}

/**
	@Function:		rsdt_free
	@Access:		Public
	@Description:
		释放 RSDT。
	@Parameters:
	@Return:
*/
void
rsdt_free(void)
{
	if(rsdt != NULL)
		free_memory(rsdt);
}

/**
	@Function:		rsdt_get_table
	@Access:		Public
	@Description:
		获取 RSDT。
	@Parameters:
	@Return:
		struct RSDT *
			成功返回 RSDT，否则返回 NULL。		
*/
struct RSDT *
rsdt_get_table(void)
{
	return rsdt;
}

/**
	@Function:		rsdt_find_sdt
	@Access:		Public
	@Description:
		查找一个指定的 SDT。
	@Parameters:
	@Return:
		struct ACPISDTHeader *
			成功返回指定的 SDT，否则返回 NULL。		
*/
struct ACPISDTHeader *
rsdt_find_sdt(IN int8 * sign)
{
	if(sign == NULL || rsdt == NULL)
		return NULL;
	uint32 count = rsdt->header.length - sizeof(struct ACPISDTHeader);
	uint32 * item = &(rsdt->first_sdt);
	uint32 ui;
	for(ui = 0; ui < count; ui++)
	{
		struct ACPISDTHeader * sdt = (struct ACPISDTHeader *)item[ui];
		if(strncmp(sdt->sign, sign, 4) == 0)
			return sdt;
	}
	return NULL;
}
