/**
	@File:			serial.c
	@Author:		Ihsoh
	@Date:			2015-02-26
	@Description:
		串行通信。
*/

#include "serial.h"
#include "types.h"

/**
	@Function:		_SrlEnable
	@Access:		Private
	@Description:
		启用指定COM口。
	@Parameters:
		port, uint16, IN
			COM口的对应端口号。
			常量：PORT_COM1，PORT_COM2，PORT_COM3，PORT_COM4。
	@Return:	
*/
static
void
_SrlEnable(IN uint16 port)
{
	KnlOutByte(port + 1, 0x00);    // Disable all interrupts
	KnlOutByte(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	KnlOutByte(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	KnlOutByte(port + 1, 0x00);    //                  (hi byte)
	KnlOutByte(port + 3, 0x03);    // 8 bits, no parity, one stop bit
	KnlOutByte(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	KnlOutByte(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

/**
	@Function:		SrlWrite
	@Access:		Public
	@Description:
		向指定COM口写一个字符。
	@Parameters:
		port, uint16, IN
			COM口的对应端口号。
			常量：PORT_COM1，PORT_COM2，PORT_COM3，PORT_COM4。
		c, int8, IN
			字符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
SrlWrite(	IN uint16 port,
			IN int8 c)
{
	while(!(KnlInByte(port + 5) & 0x20));
	KnlOutByte(port, c);
	if(c == '\n')
		SrlWrite(port, '\r');
	return TRUE;
}

/**
	@Function:		SrlWriteBuffer
	@Access:		Public
	@Description:
		向指定COM口写一个字符序列。
	@Parameters:
		port, uint16, IN
			COM口的对应端口号。
			常量：PORT_COM1，PORT_COM2，PORT_COM3，PORT_COM4。
		buffer, int8 *, IN
			储存字符序列的缓冲区。
		count, uint32, IN
			缓冲区内字符的数量。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
SrlWriteBuffer(	IN uint16 port,
				IN int8 * buffer,
				IN uint32 count)
{
	if(buffer == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < count; ui++)
		if(!SrlWrite(port, buffer[ui]))
			return FALSE;
	return TRUE;
}

/**
	@Function:		SrlHasData
	@Access:		Public
	@Description:
		确认指定COM口是否有数据。
	@Parameters:
		port, uint16, IN
			COM口的对应端口号。
			常量：PORT_COM1，PORT_COM2，PORT_COM3，PORT_COM4。
	@Return:
		BOOL
			返回TRUE则有，否则没有。
*/
BOOL
SrlHasData(IN uint16 port)
{
	return (KnlInByte(port + 5) & 1) != 0;
}

/**
	@Function:		SrlRead
	@Access:		Public
	@Description:
		尝试从COM口读取一个字符。
		如果没有字符则返回FALSE，该函数不会堵塞。
		如果有字符则返回TRUE。
	@Parameters:
		port, uint16, IN
			COM口的对应端口号。
			常量：PORT_COM1，PORT_COM2，PORT_COM3，PORT_COM4。
		c, int8 *, OUT
			用于保存一个字符的缓冲区。
	@Return:
		BOOL
			返回TRUE则有，否则没有。
*/
BOOL
SrlRead(IN uint16 port,
		OUT int8 * c)
{
	if(SrlHasData(port))
	{
		*c = KnlInByte(port);
		return TRUE;
	}
	else
		return FALSE;
}

/**
	@Function:		SrlInit
	@Access:		Public
	@Description:
		初始化所有COM口。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
SrlInit(void)
{
	_SrlEnable(PORT_COM1);
	_SrlEnable(PORT_COM2);
	_SrlEnable(PORT_COM3);
	_SrlEnable(PORT_COM4);
	return TRUE;
}
