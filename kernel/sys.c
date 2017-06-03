/**
	@File:			sys.c
	@Author:		Ihsoh
	@Date:			2017-6-3
	@Description:
		系统相关函数。
*/

#include "sys.h"
#include "cmos.h"
#include "time.h"
#include "config.h"

/**
	@Function:		KnlGetCurrentDateTime
	@Access:		Public
	@Description:
		获取当前时间。
	@Parameters:
		dt, DateTimePtr, OUT
			指向DateTime实例。
	@Return:
		BOOL
			返回TRUE则获取成功，否则失败。
*/
BOOL
KnlGetCurrentDateTime(
	OUT DateTimePtr dt)
{
	if (dt == NULL)
	{
		goto err;
	}
	struct CMOSDateTime cdt;
	if (!CmosGetDateTime(&cdt))
	{
		goto err;
	}

	int32 offset = 0;
	int8 val[10];
	if(!config_system_get_string("Timezone", val, sizeof(val)))
	{
		goto err;
	}
	if (val[0] == 'U' && val[1] == 'T' && val[2] == 'C'
		&& (val[3] == '+' || val[3] == '-')
		&& (val[4] >= '0' && val[4] <= '9')
		&& (val[5] >= '0' && val[5] <= '9')
		&& val[6] == ':'
		&& (val[7] >= '0' && val[7] <= '9')
		&& (val[8] >= '0' && val[8] <= '9')
		&& val[9] == '\0')
	{
		offset = val[3] == '+' ? 1 : -1;
		int32 hour = (val[4] - '0') * 10 + (val[5] - '0');
		if (hour > 23)
		{
			goto err;
		}
		int32 minute = (val[7] - '0') * 10 + (val[8] - '0');
		if (minute > 59)
		{
			goto err;
		}
		offset = offset * (hour * 60 * 60 + minute * 60);
		if (!KnlGetDateTime(dt, offset, cdt.year, cdt.month, cdt.day, cdt.hour, cdt.minute, cdt.second))
		{
			goto err;
		}
	}
	else
	{
		goto err;
	}

	return TRUE;
err:
	return FALSE;
}
