/**
	@File:			kfuncs.c
	@Author:		Ihsoh
	@Date:			2014-7-3
	@Description:
		内核功能。
*/

#include "types.h"
#include "kfuncs.h"
#include "386.h"

/**
	@Function:		get_memory_size
	@Access:		Public
	@Description:
		获取内存大小。
	@Parameters:
	@Return:
		uint64
			内存大小。单位是Byte。	
*/
uint64
get_memory_size(void)
{
	uint16 ax;
	uint16 bx;
	uint64 memory_byte;
	asm("pushl	%%eax\n\t"
		"pushl	%%ebx\n\t"
		"movb	$0x0, %%ah\n\t"
		"int	$0x22\n\t"
		"movw	%%ax, %0\n\t"
		"movw	%%bx, %1\n\t"
		"popl	%%ebx\n\t"
		"popl	%%eax\n\t"
		:"=r"(ax), "=r"(bx)
		:
		:"eax", "ebx");
	memory_byte = (1 * 1024 * 1024) + ((ullong)ax * 1024) + ((ullong)bx * 64 * 1024);
	return memory_byte;
}

/**
	@Function:		get_gdt_addr
	@Access:		Public
	@Description:
		获取 GDT 的起始地址。
	@Parameters:
	@Return:
		uint32
			GDT 的起始地址。	
*/
uint32
get_gdt_addr(void)
{
	uint32 eax;
	asm("pushl	%%eax\n\t"
		"movb	$0x1, %%ah\n\t"
		"int	$0x22\n\t"
		"movl	%%eax, %0\n\t"
		"popl	%%eax\n\t"
		:"=r"(eax)
		:
		:"eax");
	return eax;
}

/**
	@Function:		get_idt_addr
	@Access:		Public
	@Description:
		获取 IDT 的起始地址。
	@Parameters:
	@Return:
		uint32
			IDT 的起始地址。	
*/
uint32
get_idt_addr(void)
{
	uint32 eax;
	asm("pushl	%%eax\n\t"
		"movb	$0x2, %%ah\n\t"
		"int	$0x22\n\t"
		"movl	%%eax, %0\n\t"
		"popl	%%eax\n\t"
		:"=r"(eax)
		:
		:"eax");
	return eax;
}

/**
	@Function:		get_vesa_info_addr
	@Access:		Public
	@Description:
		获取 VESA 的信息结构体的起始地址。
		并且获取当前图形模式。
	@Parameters:
		mode, uint32 *, OUT
			当前图形模式。
	@Return:
		uint32
			VESA 的信息结构体的起始地址。	
*/
uint32
get_vesa_info_addr(OUT uint32 * mode)
{
	uint32 eax, ebx;
	asm("pushl	%%eax\n\t"
		"pushl	%%ebx\n\t"
		"movb	$0x3, %%ah\n\t"
		"int	$0x22\n\t"
		"movl	%%eax, %0\n\t"
		"movl	%%ebx, %1\n\t"
		"popl	%%ebx\n\t"
		"popl	%%eax\n\t"
		:"=r"(eax), "=r"(ebx)
		:
		:"eax", "ebx");
	*mode = ebx & 0x0FFF;
	return eax;
}

/**
	@Function:		get_kernel_tss_addr
	@Access:		Public
	@Description:
		获取内核的 TSS 的起始地址。
	@Parameters:
	@Return:
		uint32
			内核的 TSS 的起始地址。		
*/
uint32
get_kernel_tss_addr(void)
{
	uint32 eax;
	asm("pushl	%%eax\n\t"
		"movb	$0x4, %%ah\n\t"
		"int	$0x22\n\t"
		"movl	%%eax, %0\n\t"
		"popl	%%eax\n\t"
		:"=r"(eax)
		:
		:"eax");
	return eax;
}
