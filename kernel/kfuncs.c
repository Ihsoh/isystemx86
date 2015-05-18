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
#include "knlldr.h"

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
	ax = *(uint16 *)KNLLDR_HEADER_ITEM(KNLLDR_HEADER_MEMORY_SIZE_AX);
	bx = *(uint16 *)KNLLDR_HEADER_ITEM(KNLLDR_HEADER_MEMORY_SIZE_BX);
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
	return (uint32)KNLLDR_HEADER_ITEM(KNLLDR_HEADER_GDT);
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
	return (uint32)KNLLDR_HEADER_ITEM(KNLLDR_HEADER_IDT);
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
	if((uint32)KNLLDR_HEADER_ITEM(KNLLDR_HEADER_VESA_ENABLED))
	{
		*mode = (uint32)KNLLDR_HEADER_ITEM(KNLLDR_HEADER_VESA_MODE) & 0x00000fff;
		return (uint32)KNLLDR_HEADER_ITEM(KNLLDR_HEADER_VESA_INFO);
	}
	else
		return 0;
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
	return (uint32)KNLLDR_HEADER_ITEM(KNLLDR_HEADER_KERNEL_TSS);
}
