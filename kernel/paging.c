/**
	@File:			paging.c
	@Author:		Ihsoh
	@Date:			2014-11-24
	@Description:
		提供分页功能。
*/

#include "paging.h"
#include "types.h"
#include "memory.h"
#include "386.h"
#include "kfuncs.h"
#include "lock.h"
#include "log.h"

static uint32 kernel_cr3;
static uint32 * kernel_pagedt;	

DEFINE_LOCK_IMPL(paging)

#define	table_item(address, d, a, us, rw, p)	\
	((((address) << 12) & 0xfffff000) | (d << 6) | (a << 5) | (us << 2) | (rw << 1) | (p))

/**
	@Function:		_create_empty_user_pagedt
	@Access:		Private
	@Description:
		创建空的用户页目录表。
	@Parameters:
		cr3, uint32 *, OUT
			用于填充 CR3 的值。
	@Return:
		uint32 *
			页目录表真正的起始地址。	
*/
static
uint32 *
_create_empty_user_pagedt(OUT uint32 * cr3)
{
	uint32 * pagedt_ptr = alloc_memory(KB(4) + 1 * KB(4) + 1024 * KB(4) + KB(4));
	if(pagedt_ptr == NULL)
		return NULL;
	uint32 * real_pagedt_ptr = (uint32 *)((uint32)pagedt_ptr + (KB(4) - (uint32)pagedt_ptr % KB(4)));
	*cr3 = (uint32)real_pagedt_ptr;
	uint32 ui, ui1;
	//设置页目录表
	for(ui = 0; ui < TABLE_ITEM_COUNT; ui++)
		real_pagedt_ptr[ui] = table_item(	((*cr3 + KB(4) + ui * KB(4)) >> 12) & 0x000fffff, 0, 0, 
											US_USER, RW_RWE, P_VALID);
	//设置页表
	for(ui = 0; ui < TABLE_ITEM_COUNT; ui++)
		for(ui1 = 0; ui1 < TABLE_ITEM_COUNT; ui1++)
		{
			real_pagedt_ptr[TABLE_ITEM_COUNT + ui * TABLE_ITEM_COUNT + ui1] 
				= table_item(	ui * TABLE_ITEM_COUNT + ui1, 0, 0, 
								US_USER, RW_RWE, P_INVALID);
		}
	return pagedt_ptr;
}

/**
	@Function:		create_empty_user_pagedt
	@Access:		Public
	@Description:
		创建空的用户页目录表。
	@Parameters:
		cr3, uint32 *, OUT
			用于填充 CR3 的值。
	@Return:
		uint32 *
			页目录表真正的起始地址。	
*/
uint32 *
create_empty_user_pagedt(OUT uint32 * cr3)
{
	lock();
	uint32 * r = _create_empty_user_pagedt(cr3);
	unlock();
	return r;
}

/**
	@Function:		_map_user_pagedt_with_rw
	@Access:		Private
	@Description:
		修改页目录表的映射关系。
	@Parameters:
		cr3, uint32 *, OUT
			CR3 所指向的页目录表的地址。
		start, uint32, IN
			线性地址的起始地址。
		length, uint32, IN
			要映射的长度。
		real_address, uint32, IN
			要映射到的物理地址。
		rw, uint32, IN
			页的读写权限。RW_RWE(读/写/运行) 或 RW_RE(读/运行)
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_map_user_pagedt_with_rw(	OUT uint32 * cr3,
							IN uint32 start,
							IN uint32 length,
							IN uint32 real_address,
							IN uint32 rw)
{
	uint32 ui;
	uint32 dir_index = (start >> 22) & 0x3ff;
	uint32 page_index = (start >> 12) & 0x3ff;
	uint32 start_low_12bits = start & 0xfff;
	uint32 page_count = 0;
	if(length / KB(4) != 0)
	{
		page_count = length / KB(4);
		if(length % KB(4) != 0)
			page_count++;
	}
	else
		if(length != 0)
			page_count = 1;
		else
			page_count = 0;
	if(start_low_12bits != 0 || page_count == 0)	//内存起始地址和长度必须4KB对齐
		return FALSE;
	for(ui = 0; ui < page_count; ui++)
	{
		cr3[TABLE_ITEM_COUNT + dir_index * TABLE_ITEM_COUNT + page_index] 
			= table_item(	real_address >> 12, 0, 0, 
							US_USER, rw, P_VALID);
		real_address += KB(4);
		if(++page_index >= TABLE_ITEM_COUNT)
		{
			page_index = 0;
			if(++dir_index >= TABLE_ITEM_COUNT)
				return FALSE;
		}
	}
	return TRUE;
}

/**
	@Function:		map_user_pagedt_with_rw
	@Access:		Public
	@Description:
		修改页目录表的映射关系。
	@Parameters:
		cr3, uint32 *, OUT
			CR3 所指向的页目录表的地址。
		start, uint32, IN
			线性地址的起始地址。
		length, uint32, IN
			要映射的长度。
		real_address, uint32, IN
			要映射到的物理地址。
		rw, uint32, IN
			页的读写权限。RW_RWE(读/写/运行) 或 RW_RE(读/运行)
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
map_user_pagedt_with_rw(OUT uint32 * cr3,
						IN uint32 start,
						IN uint32 length,
						IN uint32 real_address,
						IN uint32 rw)
{
	lock();
	BOOL r = _map_user_pagedt_with_rw(cr3, start, length, real_address, rw);
	unlock();
	return r;
}

/**
	@Function:		_find_free_pages
	@Access:		Private
	@Description:
		寻找空闲的页。
	@Parameters:
		cr3, uint32 *, IN
			CR3 所指向的页目录表的地址。
		length, uint32, IN
			长度。
		start, uint32 *, OUT
			线性地址的起始地址。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_find_free_pages(	IN uint32 * cr3,
					IN uint32 length,
					OUT uint32 * start)
{
	uint32 request_page_count = 0;
	uint32 dir_index = 0;
	uint32 page_index = 0;
	uint32 count = 0;
	uint32 start_address = 0x00000000;
	if(length / KB(4) != 0)
	{
		request_page_count = length / KB(4);
		if(length % KB(4) != 0)
			request_page_count++;
	}
	else
		if(length != 0)
			request_page_count = 1;
		else
			request_page_count = 0;
	while(1)
	{
		uint32 page_item = cr3[TABLE_ITEM_COUNT + dir_index * TABLE_ITEM_COUNT + page_index];	
		if((page_item & 0x00000001) == P_INVALID)
		{
			if(start_address == 0x00000000)
				start_address = dir_index * (TABLE_ITEM_COUNT * KB(4)) + page_index * KB(4);
			count++;
		}
		else
		{
			start_address = 0x00000000;
			count = 0;
		}		
		if(count == request_page_count)
		{
			*start = start_address;
			return TRUE;
		}
		if(++page_index >= TABLE_ITEM_COUNT)
		{
			page_index = 0;
			if(++dir_index >= TABLE_ITEM_COUNT)
				return FALSE;
		}
	}
	return FALSE;
}

/**
	@Function:		find_free_pages
	@Access:		Public
	@Description:
		寻找空闲的页。
	@Parameters:
		cr3, uint32 *, IN
			CR3 所指向的页目录表的地址。
		length, uint32, IN
			长度。
		start, uint32 *, OUT
			线性地址的起始地址。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
find_free_pages(IN uint32 * cr3,
				IN uint32 length,
				IN uint32 * start)
{
	lock();
	BOOL r = _find_free_pages(cr3, length, start);
	unlock();
	return r;
}

/**
	@Function:		_free_pages
	@Access:		Private
	@Description:
		释放页。
	@Parameters:
		cr3, uint32 *, OUT
			CR3 所指向的页目录表的地址。
		start, uint32, IN
			起始地址。
		length, uint32, IN
			长度。
	@Return:
		类型
			返回值说明		
*/
static
BOOL
_free_pages(OUT uint32 * cr3,
			IN uint32 start,
			IN uint32 length)
{
	uint32 free_page_count = 0;
	uint32 dir_index = (start >> 22) & 0x3ff;
	uint32 page_index = (start >> 12) & 0x3ff;
	uint32 ui;
	if(length / KB(4) != 0)
	{
		free_page_count = length / KB(4);
		if(length % KB(4) != 0)
			free_page_count++;
	}
	else
		if(length != 0)
			free_page_count = 1;
		else
			free_page_count = 0;
	for(ui = 0; ui < free_page_count; ui++)
	{
		cr3[TABLE_ITEM_COUNT + dir_index * TABLE_ITEM_COUNT + page_index] 
			= table_item(	0, 0, 0,
							US_USER, RW_RWE, P_INVALID);
		if(++page_index >= TABLE_ITEM_COUNT)
		{
			page_index = 0;
			if(++dir_index >= TABLE_ITEM_COUNT && ui + 1 < free_page_count)
				return FALSE;
		}
	}
	return TRUE;
}

/**
	@Function:		free_pages
	@Access:		Public
	@Description:
		释放页。
	@Parameters:
		cr3, uint32 *, OUT
			CR3 所指向的页目录表的地址。
		start, uint32, IN
			起始地址。
		length, uint32, IN
			长度。
	@Return:
		类型
			返回值说明		
*/
BOOL
free_pages(	OUT uint32 * cr3,
			IN uint32 start,
			IN uint32 length)
{
	lock();
	BOOL r = _free_pages(cr3, start, length);
	unlock();
	return r;
}

/**
	@Function:		create_kernel_pagedt
	@Access:		Private
	@Description:
		创建内核的页目录表。
	@Parameters:
	@Return:	
*/
static
void
create_kernel_pagedt(void)
{
	kernel_pagedt = alloc_memory(KB(4) + 1 * KB(4) + 1024 * KB(4) + KB(4));
	uint32 kpagedt = (uint32)kernel_pagedt;
	kernel_cr3 = kpagedt + (KB(4) - kpagedt % KB(4)); 
	uint32 * kernel_cr3_ptr = (uint32 *)kernel_cr3;
	uint32 ui, ui1;
	//设置页目录表
	for(ui = 0; ui < TABLE_ITEM_COUNT; ui++)
		kernel_cr3_ptr[ui] = table_item(	((kernel_cr3 + KB(4) + ui * KB(4)) >> 12) & 0x000fffff, 0, 0, 
											US_USER, RW_RWE, P_VALID);
	//设置页表
	for(ui = 0; ui < TABLE_ITEM_COUNT; ui++)
		for(ui1 = 0; ui1 < TABLE_ITEM_COUNT; ui1++)
		{
			kernel_cr3_ptr[TABLE_ITEM_COUNT + ui * TABLE_ITEM_COUNT + ui1] 
				= table_item(	ui * TABLE_ITEM_COUNT + ui1, 0, 0, 
								US_USER, RW_RWE, P_VALID);
		}
}

/**
	@Function:		init_paging
	@Access:		Public
	@Description:
		初始化分页功能。
	@Parameters:
	@Return:		
*/
void
init_paging(void)
{
	create_kernel_pagedt();
	asm volatile (
		"pushal\n\t"
		"movl	%0, %%eax\n\t"
		"movl	%%eax, %%cr3\n\t"
		"popal\n\t"
	:
	:"m"(kernel_cr3));
	struct TSS * kernel_tss = get_kernel_tss_addr();
	kernel_tss->cr3 = kernel_cr3;
}

/**
	@Function:		get_kernel_cr3
	@Access:		Public/Private
	@Description:
		获取内核的页目录表的地址。	
	@Parameters:
	@Return:
		uint32
			内核的页目录表的地址。	
*/
uint32
get_kernel_cr3(void)
{
	return kernel_cr3;
}

/**
	@Function:		enable_paging
	@Access:		Public
	@Description:
		开启分页功能。
	@Parameters:
	@Return:	
*/
void
enable_paging(void)
{
	asm volatile (
		"pushal\n\t"
		"movl	%cr0, %eax\n\t"
		"orl	$0x80000000, %eax\n\t"
		"movl	%eax, %cr0\n\t"
		"popal\n\t");
}
