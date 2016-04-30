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

#include <ilib/string.h>

static struct RSDT * rsdt = NULL;

/**
	@Function:		RsdtInit
	@Access:		Public
	@Description:
		初始化 RSDT。
	@Parameters:
	@Return:
		BOOL
			初始化成功则返回 TRUE，否则返回 FALSE。
*/
BOOL
RsdtInit(void)
{
	if(rsdt != NULL)
		return TRUE;
	if(!RsdpInit())
		return FALSE;
	struct RSDPDescriptor2_0 * desc = RsdpGetDesc();
	if(desc == NULL)
		return FALSE;
	struct ACPISDTHeader * header = (struct ACPISDTHeader *)desc->part1_0.rsdt_addr;
	if(header == NULL)
		return FALSE;
	rsdt = MemAlloc(header->length);
	memcpy(rsdt, header, header->length);
	return TRUE;
}

/**
	@Function:		RsdtFree
	@Access:		Public
	@Description:
		释放 RSDT。
	@Parameters:
	@Return:
*/
void
RsdtFree(void)
{
	if(rsdt != NULL)
		MemFree(rsdt);
}

/**
	@Function:		RsdtGetTable
	@Access:		Public
	@Description:
		获取 RSDT。
	@Parameters:
	@Return:
		struct RSDT *
			成功返回 RSDT，否则返回 NULL。		
*/
struct RSDT *
RsdtGetTable(void)
{
	return rsdt;
}

/**
	@Function:		RsdtFindSDT
	@Access:		Public
	@Description:
		查找一个指定的 SDT。
	@Parameters:
	@Return:
		struct ACPISDTHeader *
			成功返回指定的 SDT，否则返回 NULL。		
*/
struct ACPISDTHeader *
RsdtFindSDT(IN int8 * sign)
{
	if(sign == NULL || rsdt == NULL)
		return NULL;
	uint32 count = (rsdt->header.length - sizeof(struct ACPISDTHeader)) / 4;
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
