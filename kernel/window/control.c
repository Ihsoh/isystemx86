/**
	@File:			control.c
	@Author:		Ihsoh
	@Date:			2015-08-31
	@Description:
		窗体控件。
*/

#include "control.h"

#include "../types.h"

static int32 _count = 0;

void
__dummy_event(uint32 id, uint32 type, void * param)
{
}

int32
__new_control_id(void)
{
	return _count++;
}
