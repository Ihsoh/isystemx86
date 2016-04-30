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
	@Function:		FadtInit
	@Access:		Public
	@Description:
		初始化 FADT。
	@Parameters:
	@Return:
		BOOL
			初始化成功则返回 TRUE，否则返回 FALSE。
*/
BOOL
FadtInit(void)
{
	if(is_inited)
		return TRUE;
	if(!RsdtInit())
		return FALSE;
	memcpy(&fadt, RsdtFindSDT("FACP"), sizeof(struct FADT));
	is_inited = TRUE;
	return TRUE;
}

/**
	@Function:		FadtGetTable
	@Access:		Public
	@Description:
		获取 FADT。
	@Parameters:
	@Return:
		struct FADT *
			成功返回 FADT，否则返回 NULL。		
*/
struct FADT *
FadtGetTable(void)
{
	if(!is_inited)
		return NULL;
	return &fadt;
}
