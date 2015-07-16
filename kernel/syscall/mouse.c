/**
	@File:			mouse.c
	@Author:		Ihsoh
	@Date:			2014-11-14
	@Description:
		鼠标的系统调用处理程序。
*/

#include "mouse.h"
#include "../types.h"
#include "../sparams.h"
#include "../kernel.h"

/**
	@Function:		system_call_mouse
	@Access:		Public
	@Description:
		鼠标的系统调用处理程序。
	@Parameters:
		func, uint32, IN
			功能号。
		base, uint32, IN
			任务的基地址。该地址为物理地址。
		sparams, struct SParams *, IN OUT
			系统调用参数。
	@Return:	
*/
void
system_call_mouse(	IN uint32 func,
					IN uint32 base,
					IN OUT struct SParams * sparams)
{
	switch(func)
	{
		//获取鼠标坐标
		//
		//参数:
		//	Param0=X
		//	Param1=Y
		case SCALL_GET_MPOSITION:
		{
			int32 x, y;
			get_mouse_position(&x, &y);
			sparams->param0 = SPARAM(x);
			sparams->param1 = SPARAM(y);
			break;
		}
		//获取鼠标左键状态
		//
		//参数:
		//	Param0=0:未按下, 1:按下
		case SCALL_GET_MLBSTATE:
		{
			BOOL r = is_mouse_left_button_down();
			sparams->param0 = SPARAM(r);
			break;
		}
		//获取鼠标右键状态
		//
		//参数:
		//	Param0=0:未按下, 1:按下
		case SCALL_GET_MRBSTATE:
		{
			BOOL r = is_mouse_right_button_down();
			sparams->param0 = SPARAM(r);
			break;
		}
	}
}
