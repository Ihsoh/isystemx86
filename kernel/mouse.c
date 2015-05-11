/**
	@File:			mouse.c
	@Author:		Ihsoh
	@Date:			2015-05-12
	@Description:
		提供鼠标的功能。
*/

#include "mouse.h"
#include "types.h"

BOOL
mouse_init(void)
{
	//允许鼠标接口
	outb(0x64, 0xa8);
	//通知8042下一个字节的0x60的数据发送给鼠标
	outb(0x64, 0xd4);
	//允许鼠标发送数据
	outb(0x60, 0xf4);
	//通知8042下一个字节发向0x60的数据应方向8042的命令寄存器
	outb(0x64, 0x60);
	//许可键盘及鼠标接口及中断
	outb(0x60, 0x47);
	return TRUE;
}
