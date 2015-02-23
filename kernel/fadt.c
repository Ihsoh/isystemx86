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

struct FADT *
fadt_get_table(void)
{
	if(!is_inited)
		return NULL;
	return &fadt;
}
