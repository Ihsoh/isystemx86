/**
	@File:			386.c
	@Author:		Ihsoh
	@Date:			2014-1-28
	@Description:
		提供与CPU相关的功能。
*/

#include "386.h"
#include "types.h"
#include <ilib/string.h>

/**
	@Function:		KnlOutByte
	@Access:		Public
	@Description:
		向指定端口输出一个字节的数据。
	@Parameters:
		port, uint16, IN
			端口号。
		data, uint8, IN
			数据。
	@Return:		
*/
void
KnlOutByte(	IN uint16 port,
		IN uint8 data)
{
	asm volatile (
		"outb	%%al, %%dx\n\t"
		"jmp	1f\n\t"
		"1:"
		"jmp	1f\n\t"
		"1:"			
		:
		:"a"(data), "d"(port));
}

/**
	@Function:		KnlInByte
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
KnlInByte(IN uint16 port)
{
	uint8 data;
	asm volatile ("inb	%%dx, %%al"
		:"=a"(data)
		:"d"(port));
	return data;
}

/**
	@Function:		KnlOutWord
	@Access:		Public
	@Description:
		向指定端口输出一个字的数据。
	@Parameters:
		port, uint16, IN
			端口号。
		data, uint16, IN
			数据。
	@Return:		
*/
void 
KnlOutWord(	IN uint16 port,
		IN uint16 data)
{
	asm volatile (
		"outw	%%ax, %%dx\n\t"
		"jmp	1f\n\t"
		"1:"
		"jmp	1f\n\t"
		"1:"	
		:
		:"a"(data), "d"(port));
}

/**
	@Function:		KnlInWord
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
KnlInWord(IN uint16 port)
{
	uint16 data;
	asm volatile ("inw	%%dx, %%ax"
		:"=a"(data)
		:"d"(port));
	return data;
}

/**
	@Function:		KnlOutLong
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
KnlOutLong(	IN uint16 port,
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
	@Function:		KnlInLong
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
KnlInLong(IN uint16 port)
{
	uint32 data;
	asm volatile ("inl	%%dx, %%eax"
		:"=a"(data)
		:"d"(port));
	return data;
}

/**
	@Function:		KnlOutBytes
	@Access:		Public
	@Description:
		向指定端口输出指定数量的字节的数据。
	@Parameters:
		port, uint16, IN
			端口号。
		buffer, uint8 *, IN
			数据缓冲区。
		count, uint32, IN
			字节的数量。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
KnlOutBytes(IN uint16 port, 
			IN uint8 * buffer,
			IN uint32 count)
{
	if(buffer == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < count; ui++)
		KnlOutByte(port, buffer[ui]);
	return TRUE;
}

/**
	@Function:		KnlInBytes
	@Access:		Public
	@Description:
		从指定端口读取指定数量的字节的数据。
	@Parameters:
		port, uint16, IN
			端口号。
		buffer, uint8 *, OUT
			用于储存数据的缓冲区。
		count, uint32, IN
			字节的数量。
	@Return:	
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
KnlInBytes(	IN uint16 port,
			OUT uint8 * buffer,
			IN uint32 count)
{
	if(buffer == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < count; ui++)
		buffer[ui] = KnlInByte(port);
	return TRUE;
}

/**
	@Function:		KnlOutWords
	@Access:		Public
	@Description:
		向指定端口输出指定数量的字的数据。
	@Parameters:
		port, uint16, IN
			端口号。
		buffer, uint16 *, IN
			数据缓冲区。
		count, uint32, IN
			字的数量。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
KnlOutWords(IN uint16 port, 
			IN uint16 * buffer,
			IN uint32 count)
{
	if(buffer == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < count; ui++)
		KnlOutWord(port, buffer[ui]);
	return TRUE;
}

/**
	@Function:		KnlInWords
	@Access:		Public
	@Description:
		从指定端口读取指定数量的字的数据。
	@Parameters:
		port, uint16, IN
			端口号。
		buffer, uint16 *, OUT
			用于储存数据的缓冲区。
		count, uint32, IN
			字的数量。
	@Return:	
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
KnlInWords(	IN uint16 port,
			OUT uint16 * buffer,
			IN uint32 count)
{
	if(buffer == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < count; ui++)
		buffer[ui] = KnlInWord(port);
	return TRUE;
}

/**
	@Function:		KnlOutLongs
	@Access:		Public
	@Description:
		向指定端口输出指定数量的双字的数据。
	@Parameters:
		port, uint16, IN
			端口号。
		buffer, uint32 *, IN
			数据缓冲区。
		count, uint32, IN
			双字的数量。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
KnlOutLongs(IN uint16 port, 
			IN uint32 * buffer,
			IN uint32 count)
{
	if(buffer == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < count; ui++)
		KnlOutLong(port, buffer[ui]);
	return TRUE;
}

/**
	@Function:		KnlInLongs
	@Access:		Public
	@Description:
		从指定端口读取指定数量的双字的数据。
	@Parameters:
		port, uint16, IN
			端口号。
		buffer, uint32 *, OUT
			用于储存数据的缓冲区。
		count, uint32, IN
			双字的数量。
	@Return:	
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
KnlInLongs(	IN uint16 port,
			OUT uint32 * buffer,
			IN uint32 count)
{
	if(buffer == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < count; ui++)
		buffer[ui] = KnlInLong(port);
	return TRUE;
}

/**
	@Function:		KnlGetCpuMSR
	@Access:		Public
	@Description:
		读取 MSR 寄存器。
	@Parameters:
		msr, uint32, IN
			MSR Index。
		lo, uint32 *, OUT
			低32位。
		hi, uint32 *, OUT
			高32位。
	@Return:
*/
void
KnlGetCpuMSR(	IN uint32 msr,
				OUT uint32 * lo,
				OUT uint32 * hi)
{
	asm volatile("pushal");
	asm volatile(
		"rdmsr"
		:"=a"(*lo), "=d"(*hi)
		:"c"(msr));
	asm volatile("popal");
}

/**
	@Function:		KnlSetCpuMSR
	@Access:		Public
	@Description:
		设置 MSR 寄存器。
	@Parameters:
		msr, uint32, IN
			MSR Index。
		lo, uint32, IN
			低32位。
		hi, uint32, IN
			高32位。
	@Return:
*/
void
KnlSetCpuMSR(	IN uint32 msr,
				IN uint32 lo,
				IN uint32 hi)
{
	asm volatile("pushal");
	asm volatile(
		"wrmsr"
		:
		:"a"(lo), "d"(hi), "c"(msr));
	asm volatile("popal");
}

/**
	@Function:		KnlSetInterrupt
	@Access:		Public
	@Description:
		设置相应中断处理函数。
		描述符是陷阱门描述符。
	@Parameters:
		n, uint8, IN
			中断号。
		addr, uint32, IN
			处理函数的地址。
	@Return:	
*/
void
KnlSetInterrupt(IN uint8 n,
				IN uint32 addr)
{
	struct Gate gate;
	gate.offsetl = (uint16)(addr & 0x0000ffff);
	gate.offseth = (uint16)((addr >> 16) & 0x0000ffff);
	gate.dcount = 0;
	gate.selector = KERNEL_C_DESC | RPL0;
	gate.attr = AT386TGATE | DPL3;
	KnlSetGateToIDT(n, &gate);
}

/**
	@Function:		KnlSetInterruptGate
	@Access:		Public
	@Description:
		设置相应中断处理函数。
		描述符是中断门描述符。
	@Parameters:
		n, uint8, IN
			中断号。
		addr, uint32, IN
			处理函数的地址。
	@Return:	
*/
void
KnlSetInterruptGate(IN uint8 n,
					IN uint32 addr)
{
	struct Gate gate;
	gate.offsetl = (uint16)(addr & 0x0000ffff);
	gate.offseth = (uint16)((addr >> 16) & 0x0000ffff);
	gate.dcount = 0;
	gate.selector = KERNEL_C_DESC | RPL0;
	gate.attr = AT386IGATE | DPL3;
	KnlSetGateToIDT(n, &gate);
}

/**
	@Function:		KnlSetDescToGDT
	@Access:		Public
	@Description:
		设置GDT中的描述符。
	@Parameters:
		index, uint32, IN
			索引。
		desc, uint8 *, IN
			描述符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
KnlSetDescToGDT(IN uint32 index,
				IN uint8 * desc)
{
	if(index >= MAX_DESC_COUNT || desc == NULL)
		return FALSE;
	uint8 * gdt_addr = (uint8 *)get_gdt_addr();
	if(gdt_addr == NULL)
		return FALSE;
	memcpy(	gdt_addr + index * sizeof(struct Desc), 
			desc, 
			sizeof(struct Desc));
	return TRUE;
}

/**
	@Function:		KnlGetDescFromGDT
	@Access:		Public
	@Description:
		获取GDT中的描述符。
	@Parameters:
		index, uint32, IN
			索引。
		desc, uint8 *, OUT
			描述符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
KnlGetDescFromGDT(	IN uint32 index, 
					OUT uint8 * desc)
{
	if(index >= MAX_DESC_COUNT || desc == NULL)
		return FALSE;
	uint8 * gdt_addr = (uint8 *)get_gdt_addr();
	if(gdt_addr == NULL)
		return FALSE;
	memcpy(	desc, 
			gdt_addr + index * sizeof(struct Desc), 
			sizeof(struct Desc));
	return TRUE;
}

/**
	@Function:		KnlSetGateToIDT
	@Access:		Public
	@Description:
		设置IDT中的门描述符。
	@Parameters:
		index, uint32, IN
			索引。
		gate, uint8 *, IN
			门描述符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
KnlSetGateToIDT(IN uint32 index, 
				IN uint8 * gate)
{
	if(index >= MAX_DESC_COUNT || gate == NULL)
		return FALSE;
	uint8 * idt_addr = (uint8 *)get_idt_addr();
	if(idt_addr == NULL)
		return FALSE;
	memcpy(	idt_addr + index * sizeof(struct Gate), 
			gate, 
			sizeof(struct Gate));
	return TRUE;
}

/**
	@Function:		KnlGetGateFromIDT
	@Access:		Public
	@Description:
		获取IDT中的门描述符。
	@Parameters:
		index, uint32, IN
			索引。
		gate, uint8 *, OUT
			门描述符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
KnlGetGateFromIDT(	IN uint32 index,
					OUT uint8 * gate)
{
	if(index >= MAX_DESC_COUNT || gate == NULL)
		return FALSE;
	uint8 * idt_addr = (uint8 *)get_idt_addr();
	if(idt_addr == NULL)
		return FALSE;
	memcpy(	gate,
			idt_addr + index * sizeof(struct Gate),
			sizeof(struct Gate));
	return TRUE;
}

/**
	@Function:		KnlResetCpu
	@Access:		Public
	@Description:
		重置CPU。
	@Parameters:
	@Return:
*/
void
KnlResetCpu(void)
{
	asm volatile (
		"wait_ib:\n\t"
		"inb	$0x64, %al\n\t"
		"test	$0x02, %al\n\t"
		"jne	wait_ib\n\t"
		"movb	$0xfe, %al\n\t"
		"outb	%al, $0x64\n\t"
		"cli\n\t"
		"hlt\n\t");
}

/**
	@Function:		KnlHaltCpu
	@Access:		Public
	@Description:
		停止CPU。
	@Parameters:
	@Return:
*/
void
KnlHaltCpu(void)
{
	asm volatile (
		"cli\n\t"
		"hlt\n\t");
}
