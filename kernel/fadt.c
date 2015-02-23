/**
	@File:			fadt.c
	@Author:		Ihsoh
	@Date:			2015-02-24
	@Description:
		FADT。
*/

#include "fadt.h"
#include "types.h"
#include "rsdt.h"
#include "memory.h"

static struct FADT fadt;
static BOOL is_inited = FALSE;

/**
	@Function:		fadt_init
	@Access:		Public
	@Description:
		初始化 FADT。
	@Parameters:
	@Return:
		BOOL
			初始化成功则返回 TRUE，否则返回 FALSE。
*/
BOOL
fadt_init(void)
{
	if(is_inited)
		return TRUE;
	if(!rsdt_init())
		return FALSE;
	memcpy(&fadt, rsdt_find_sdt("FACP"), sizeof(struct FADT));
	is_inited = TRUE;
	return TRUE;
}

/**
	@Function:		fadt_get_table
	@Access:		Public
	@Description:
		获取 FADT。
	@Parameters:
	@Return:
		struct FADT *
			成功返回 FADT，否则返回 NULL。		
*/
struct FADT *
fadt_get_table(void)
{
	if(!is_inited)
		return NULL;
	return &fadt;
}
