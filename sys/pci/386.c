#include <ilib.h>

/**
	@Function:		inb
	@Access:		Public
	@Description:
		从指定端口读取一个字节的数据。
	@Parameters:
		port, uint16, IN
			端口号。
	@Return:	
		uint8
			数据。	
*/
uint8 
inb(IN uint16 port)
{
	uint8 data;
	asm volatile ("inb	%%dx, %%al"
		:"=a"(data)
		:"d"(port));
	return data;
}

/**
	@Function:		inw
	@Access:		Public
	@Description:
		从指定端口读取一个字的数据。
	@Parameters:
		port, uint16, IN
			端口号。
	@Return:	
		uint16
			数据。	
*/
uint16 
inw(IN uint16 port)
{
	uint16 data;
	asm volatile ("inw	%%dx, %%ax"
		:"=a"(data)
		:"d"(port));
	return data;
}

/**
	@Function:		outl
	@Access:		Public
	@Description:
		向指定端口输出一个双字的数据。
	@Parameters:
		port, uint16, IN
			端口号。
		data, uint32, IN
			数据。
	@Return:		
*/
void 
outl(	IN uint16 port,
		IN uint32 data)
{
	asm volatile (
		"outl	%%eax, %%dx\n\t"
		"jmp	1f\n\t"
		"1:"
		"jmp	1f\n\t"
		"1:"	
		:
		:"a"(data), "d"(port));
}

/**
	@Function:		inl
	@Access:		Public
	@Description:
		从指定端口读取一个双字的数据。
	@Parameters:
		port, uint16, IN
			端口号。
	@Return:	
		uint32
			数据。	
*/
uint32
inl(IN uint16 port)
{
	uint32 data;
	asm volatile ("inl	%%dx, %%eax"
		:"=a"(data)
		:"d"(port));
	return data;
}
