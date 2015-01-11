/**
	@File:			scall_keyboard.c
	@Author:		Ihsoh
	@Date:			2014-7-29
	@Description:
		键盘的系统调用处理程序。
*/

#include "scall_keyboard.h"
#include "types.h"
#include "sparams.h"
#include "keyboard.h"
#include "tasks.h"

/**
	@Function:		system_call_keyboard
	@Access:		Public
	@Description:
		键盘的系统调用处理程序。
	@Parameters:
		func, uint32, IN
			功能号。
		base, uint32, IN
			任务的基地址。该地址为物理地址。
		sparams, struct SParams *, IN OUT
			系统调用参数。
	@Return:	
*/
void system_call_keyboard(	IN uint32 func,
							IN uint32 base,
							IN OUT struct SParams * sparams)
{
	switch(func)
	{
		//读入一个字符
		//
		//返回值:
		//	Param0=读入的字符
		case SCALL_GET_CHAR:
		{
			sparams->param0 = SPARAM(get_char());
			break;
		}
		//读入字符串. 以回车结束, 但字符串不包含回车.
		//
		//参数:
		//	Param0=缓冲区地址(相对于调用程序空间的偏移地址)
		//	Param1=最大读入字符数
		//返回值:
		//	Param0=真正读入的字符数
		case SCALL_GET_STR_N:
		{
			int8 * buffer = (int8 *)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param0));
			sparams->param0 = SPARAM(get_strn(buffer, UINT32_SPARAM(sparams->param1)));
			break;
		}
		//获取Shift键的状态
		//
		//返回值:
		//	Param0=1则按下, 0则弹起
		case SCALL_GET_SHIFT:
		{
			sparams->param0 = SPARAM(get_shift());
			break;
		}
		//获取Control键的状态
		//
		//返回值:
		//	Param0=1则按下, 0则弹起
		case SCALL_GET_CONTROL:
		{
			sparams->param0 = SPARAM(get_control());
			break;
		}
		//获取Alt键的状态
		//
		//返回值:
		//	Param0=1则按下, 0则弹起
		case SCALL_GET_ALT:
		{
			sparams->param0 = SPARAM(get_alt());
			break;
		}
		//获取按键状态
		//
		//参数:
		//	Param0=按键键码
		//返回值:
		//	Param0=1则按下, 0则弹起
		case SCALL_GET_KEY_STATUS:
		{
			int32 key = INT32_SPARAM(sparams->param0);
			sparams->param0 = SPARAM(get_key_status(key));			
			break;
		}
		//键盘缓冲区是否包含输入的字符
		//
		//返回值:
		//	Param0=1则有, 0则无
		case SCALL_HAS_KEY:
		{
			sparams->param0 = SPARAM(has_key());		
			break;
		}
	}
}
