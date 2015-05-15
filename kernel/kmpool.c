/**
	@File:			kmpool.c
	@Author:		Ihsoh
	@Date:			2015-05-14
	@Description:
		内核内存池。
		这个内存池是通过libs/mempoollib实现的。
		使用kmpool_malloc或kmpool_calloc分配的内存块必须使用kmpool_free释放内存。
*/

#include "kmpool.h"
#include "types.h"

#include <mempoollib/mempoollib.h>

static MEMPOOLLPool		_kmpool;							// 内存池对象。
static BOOL				_kmpool_is_enabled	= FALSE;		// 内存池是否可用。

/**
	@Function:		kmpool_init
	@Access:		Public
	@Description:
		初始化内核内存池。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
kmpool_init(void)
{
	if(_kmpool_is_enabled)
		return TRUE;
	if(!mempooll_init_pool(&_kmpool))
		return FALSE;
	_kmpool_is_enabled = TRUE;
	return TRUE;
}

/**
	@Function:		kmpool_malloc
	@Access:		Public
	@Description:
		使用内核内存池分配内存块。
	@Parameters:
		size, uint32, IN
			需要分配的内存块的尺寸。
	@Return:
		void *
			成功则返回内存块的指针，否则返回NULL。
*/
void *
kmpool_malloc(IN uint32 size)
{
	if(	!_kmpool_is_enabled
		|| size == 0)
		return NULL;
	return mempooll_alloc_memory(&_kmpool, size);
}

/**
	@Function:		kmpool_calloc
	@Access:		Public
	@Description:
		使用内核内存池分配内存块。
		并且可以指定需要分配内存块的数量。
		这些内存块是连续的。
		分配的总尺寸为count * size。
	@Parameters:
		count, uint32, IN
			需要分配内存块的数量。
		size, uint32, IN
			需要分配的内存块的尺寸。
	@Return:
		void *
			成功则返回指向第一个内存块的指针，否则返回NULL。
*/
void *
kmpool_calloc(	IN uint32 count,
				IN uint32 size)
{
	if(	!_kmpool_is_enabled
		|| count == 0
		|| size == 0)
		return NULL;
	return mempooll_alloc_memory(&_kmpool, count * size);
}

/**
	@Function:		kmpool_free
	@Access:		Public
	@Description:
		释放使用内核内存池分配的内存块。
	@Parameters:
		ptr, void *, IN
			指向使用内核内存池分配的内存块的指针。
	@Return:
*/
void
kmpool_free(IN void * ptr)
{
	if(ptr == NULL)
		return;
	mempooll_free_memory(&_kmpool, ptr);
}
