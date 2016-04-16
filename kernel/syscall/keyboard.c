/**
	@File:			keyboard.c
	@Author:		Ihsoh
	@Date:			2014-7-29
	@Description:
		键盘的系统调用处理程序。
*/

#include "keyboard.h"
#include "../types.h"
#include "../sparams.h"
#include "../keyboard.h"
#include "../tasks.h"

/**
	@Function:		_ScKbdProcess
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
void _ScKbdProcess(	IN uint32 func,
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
			uint8 chr = get_char_utask();
			sparams->param0 = SPARAM(chr);
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
			uint32 count = get_strn_utask(buffer, UINT32_SPARAM(sparams->param1));
			sparams->param0 = SPARAM(count);
			break;
		}
		//获取Shift键的状态
		//
		//返回值:
		//	Param0=1则按下, 0则弹起
		case SCALL_GET_SHIFT:
		{
			BOOL r = get_shift();
			sparams->param0 = SPARAM(r);
			break;
		}
		//获取Control键的状态
		//
		//返回值:
		//	Param0=1则按下, 0则弹起
		case SCALL_GET_CONTROL:
		{
			BOOL r = get_control();
			sparams->param0 = SPARAM(r);
			break;
		}
		//获取Alt键的状态
		//
		//返回值:
		//	Param0=1则按下, 0则弹起
		case SCALL_GET_ALT:
		{
			BOOL r = get_alt();
			sparams->param0 = SPARAM(r);
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
			int32 status = get_key_status(key);
			sparams->param0 = SPARAM(status);
			break;
		}
		//键盘缓冲区是否包含输入的字符
		//
		//返回值:
		//	Param0=1则有, 0则无
		case SCALL_HAS_KEY:
		{
			BOOL r = has_key();
			sparams->param0 = SPARAM(r);		
			break;
		}
	}
}
