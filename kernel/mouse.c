/**
	@File:			mouse.c
	@Author:		Ihsoh
	@Date:			2015-05-12
	@Description:
		提供鼠标的功能。
*/

#include "mouse.h"
#include "types.h"

#define PACKETS_IN_PIPE		1024
#define DISCARD_POINT		32

#define MOUSE_IRQ 		12

#define MOUSE_PORT		0x60
#define MOUSE_STATUS	0x64
#define MOUSE_ABIT		0x02
#define MOUSE_BBIT		0x01
#define MOUSE_WRITE		0xD4
#define MOUSE_F_BIT		0x20
#define MOUSE_V_BIT		0x08

#define MOUSE_DEFAULT		0
#define MOUSE_SCROLLWHEEL	1
#define MOUSE_BUTTONS		2

static
void
mouse_wait(uint8 a_type)
{
	uint32 timeout = 100000;
	if(!a_type)
	{
		while(--timeout)
			if((inb(MOUSE_STATUS) & MOUSE_BBIT) == 1)
				return;
		print_str("<mouse timeout>\n");
		asm volatile ("hlt;");
		return;
	}
	else
	{
		while(--timeout)
			if(!((inb(MOUSE_STATUS) & MOUSE_ABIT)))
				return;
		print_str("<mouse timeout>\n");
		asm volatile ("hlt;");
		return;
	}
}

static
void
mouse_write(uint8 write)
{
	mouse_wait(1);
	outb(MOUSE_STATUS, MOUSE_WRITE);
	mouse_wait(1);
	outb(MOUSE_PORT, write);
}

static
uint8
mouse_read(void)
{
	mouse_wait(0);
	int8 t = inb(MOUSE_PORT);
	return t;
}

BOOL
mouse_init(void)
{
	/*
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
	*/

	uint8 status, result;
	mouse_wait(1);
	outb(MOUSE_STATUS, 0xA8);
	mouse_wait(1);
	outb(MOUSE_STATUS, 0x20);
	mouse_wait(0);
	status = inb(0x60) | 2;
	mouse_wait(1);
	outb(MOUSE_STATUS, 0x60);
	mouse_wait(1);
	outb(MOUSE_PORT, status);
	mouse_write(0xF6);
	mouse_read();
	mouse_write(0xF4);
	mouse_read();
	mouse_write(0xF2);
	mouse_read();
	result = mouse_read();



	uint8 tmp = inb(0x61);
	outb(0x61, tmp | 0x80);
	outb(0x61, tmp & 0x7F);
	inb(0x60);

	return TRUE;
}
