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
_MouseWait(uint8 a_type)
{
	uint32 timeout = 100000;
	if(!a_type)
	{
		while(--timeout)
			if((KnlInByte(MOUSE_STATUS) & MOUSE_BBIT) == 1)
				return;
		ScrPrintString("<mouse timeout>\n");
		asm volatile ("hlt;");
		return;
	}
	else
	{
		while(--timeout)
			if(!((KnlInByte(MOUSE_STATUS) & MOUSE_ABIT)))
				return;
		ScrPrintString("<mouse timeout>\n");
		asm volatile ("hlt;");
		return;
	}
}

static
void
_MouseWrite(uint8 write)
{
	_MouseWait(1);
	KnlOutByte(MOUSE_STATUS, MOUSE_WRITE);
	_MouseWait(1);
	KnlOutByte(MOUSE_PORT, write);
}

static
uint8
_MouseRead(void)
{
	_MouseWait(0);
	int8 t = KnlInByte(MOUSE_PORT);
	return t;
}

BOOL
MouseInit(void)
{
	/*
	//允许鼠标接口
	KnlOutByte(0x64, 0xa8);
	//通知8042下一个字节的0x60的数据发送给鼠标
	KnlOutByte(0x64, 0xd4);
	//允许鼠标发送数据
	KnlOutByte(0x60, 0xf4);
	//通知8042下一个字节发向0x60的数据应方向8042的命令寄存器
	KnlOutByte(0x64, 0x60);
	//许可键盘及鼠标接口及中断
	KnlOutByte(0x60, 0x47);
	*/

	uint8 status, result;
	_MouseWait(1);
	KnlOutByte(MOUSE_STATUS, 0xA8);
	_MouseWait(1);
	KnlOutByte(MOUSE_STATUS, 0x20);
	_MouseWait(0);
	status = KnlInByte(0x60) | 2;
	_MouseWait(1);
	KnlOutByte(MOUSE_STATUS, 0x60);
	_MouseWait(1);
	KnlOutByte(MOUSE_PORT, status);
	_MouseWrite(0xF6);
	_MouseRead();
	_MouseWrite(0xF4);
	_MouseRead();
	_MouseWrite(0xF2);
	_MouseRead();
	result = _MouseRead();



	uint8 tmp = KnlInByte(0x61);
	KnlOutByte(0x61, tmp | 0x80);
	KnlOutByte(0x61, tmp & 0x7F);
	KnlInByte(0x60);

	return TRUE;
}
