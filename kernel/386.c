/**
	@File:			386.c
	@Author:		Ihsoh
	@Date:			2014-1-28
	@Description:
		提供与CPU相关的功能。
*/

#include "386.h"
#include "types.h"
#include <string.h>

/**
	@Function:		outb
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
outb(	IN uint16 port,
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
	@Function:		outw
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
outw(	IN uint16 port,
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

/**
	@Function:		get_msr
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
get_msr(IN uint32 msr,
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
	@Function:		set_msr
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
set_msr(IN uint32 msr,
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
	@Function:		set_int
	@Access:		Public
	@Description:
		设置相应中断处理函数。
	@Parameters:
		n, uint8, IN
			中断号。
		addr, uint32, IN
			处理函数的地址。
	@Return:	
*/
void
set_int(IN uint8 n,
		IN uint32 addr)
{
	asm volatile ("int	$0x21"
	:
	:"b"(addr), "d"(n));
}

/**
	@Function:		set_desc_to_gdt
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
set_desc_to_gdt(IN uint32 index,
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
	@Function:		get_desc_from_gdt
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
get_desc_from_gdt(	IN uint32 index, 
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
	@Function:		set_gate_to_idt
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
set_gate_to_idt(IN uint32 index, 
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
	@Function:		get_gate_from_idt
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
get_gate_from_idt(	IN uint32 index,
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
	@Function:		reset_cpu
	@Access:		Public
	@Description:
		重置CPU。
	@Parameters:
	@Return:
*/
void
reset_cpu(void)
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
	@Function:		halt_cpu
	@Access:		Public
	@Description:
		停止CPU。
	@Parameters:
	@Return:
*/
void
halt_cpu(void)
{
	asm volatile (
		"cli\n\t"
		"hlt\n\t");
}
