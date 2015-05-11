/**
	@File:			mempoollib.c
	@Author:		Ihsoh
	@Date:			2015-05-08
	@Description:
		内存池。
*/

#include "mempoollib.h"
#include "types.h"

MEMPOOLLMalloc	mempooll_malloc	= NULL;
MEMPOOLLCalloc	mempooll_calloc	= NULL;
MEMPOOLLFree	mempooll_free	= NULL;

/**
	@Function:		mempooll_init
	@Access:		Public
	@Description:
		初始化内存池库。
	@Parameters:
		env, MEMPOOLLEnvironmentPtr, IN
			指向内存池环境的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
mempooll_init(IN MEMPOOLLEnvironmentPtr env)
{
	if(	env == NULL
		|| env->mempooll_malloc == NULL
		|| env->mempooll_calloc == NULL
		|| env->mempooll_free == NULL)
		return FALSE;
	mempooll_malloc = env->mempooll_malloc;
	mempooll_calloc = env->mempooll_calloc;
	mempooll_free = env->mempooll_free;
	return TRUE;
}

/**
	@Function:		mempooll_shift
	@Access:		Private
	@Description:
		从MEMPOOLLLinkedList的头部弹出一个MEMPOOLLBlock。
	@Parameters:
		list, MEMPOOLLLinkedListPtr, IN OUT
			指向MEMPOOLLLinkedList的指针。
	@Return:
		MEMPOOLLBlockPtr
			如果成功弹出一个MEMPOOLLBlock，则返回指向该MEMPOOLLBlock的指针。
			否则返回NULL。
*/
static
MEMPOOLLBlockPtr
mempooll_shift(IN OUT MEMPOOLLLinkedListPtr list)
{
	if(	list == NULL 
		|| list->head == NULL
		|| list->foot == NULL
		|| list->count == 0)
		return NULL;
	MEMPOOLLBlockPtr next = list->head->next;
	MEMPOOLLBlockPtr block = list->head;
	if(next == NULL)
	{
		list->head = NULL;
		list->foot = NULL;
	}
	else
	{
		list->head = next;
		next->prev = NULL;
	}
	block->prev = NULL;
	block->next = NULL;
	list->count--;
	return block;
}

/**
	@Function:		mempooll_unshift
	@Access:		Private
	@Description:
		在MEMPOOLLLinkedList的头部压入一个MEMPOOLLBlock。
	@Parameters:
		list, MEMPOOLLLinkedListPtr, IN OUT
			指向MEMPOOLLLinkedList的指针。
		block, MEMPOOLLBlockPtr, IN
			指向MEMPOOLLBlock的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
mempooll_unshift(	IN OUT MEMPOOLLLinkedListPtr list,
					IN MEMPOOLLBlockPtr block)
{
	if(	list == NULL
		|| block == NULL)
		return FALSE;
	if(list->head == NULL && list->foot == NULL)
	{
		list->head = block;
		list->foot = block;
		block->prev = NULL;
		block->next = NULL;
	}
	else if(list->head != NULL && list->foot != NULL)
	{
		block->next = list->head;
		list->head->prev = block;
		block->prev = NULL;
		list->head = block;
	}
	else
		return FALSE;
	list->count++;
	return TRUE;
}

/**
	@Function:		mempooll_remove
	@Access:		Private
	@Description:
		从MEMPOOLLLinkedList移除一个MEMPOOLLBlock。
	@Parameters:
		list, MEMPOOLLLinkedListPtr, IN OUT
			指向MEMPOOLLLinkedList的指针。
		block, MEMPOOLLBlockPtr, IN
			指向MEMPOOLLBlock的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
mempooll_remove(IN OUT MEMPOOLLLinkedListPtr list,
				IN MEMPOOLLBlockPtr block)
{
	if(	list == NULL
		|| block == NULL
		|| list->count == 0)
		return FALSE;
	if(list->head == block && list->foot == block)
	{
		list->head = NULL;
		list->foot = NULL;
	}
	else if(list->head == block && list->foot != block)
	{
		block->next->prev = NULL;
		list->head = block->next;
	}
	else if(list->head != block && list->foot == block)
	{
		block->prev->next = NULL;
		list->foot = block->prev;
	}
	else if(list->head != block && list->foot != block)
	{
		block->prev->next = block->next;
		block->next->prev = block->prev;
	}
	else
		return FALSE;
	list->count--;
	return TRUE;
}

/*
	内存池能分配的内存块的大小。
	每个元素的单位是Byte。
*/
static uint32 sizes[MEMPOOLL_LIST_COUNT]
	= {	4, 8, 16, 32, 
		64, 128, 256, 512, 
		KB(1), KB(2), KB(3), KB(4),
		KB(5), KB(6), KB(7), KB(8)};

#define	MEM_LIST_INC_SIZE	(KB(16))

/**
	@Function:		mempooll_mem_list_add
	@Access:		Private
	@Description:
		向pool->mem_list添加指针。
		这些指针是由于需要填充free列表而分配的内存块的指针。
	@Parameters:
		pool, MEMPOOLLPoolPtr, IN OUT
			指向MEMPOOLLPool的指针。
		ptr, void *, IN
			指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
mempooll_mem_list_add(	IN OUT MEMPOOLLPoolPtr pool,
						IN void * ptr)
{
	if(pool == NULL || ptr == NULL)
		return FALSE;
	if(pool->mem_list_count < pool->mem_list_max_count)
		pool->mem_list[pool->mem_list_count++] = ptr;
	else
	{
		uint32 new_size = pool->mem_list_max_count * 4 + MEM_LIST_INC_SIZE;
		uint32 new_max_count = pool->mem_list_max_count + MEM_LIST_INC_SIZE / 4;
		void ** new_mem_list = mempooll_malloc(new_size);
		if(new_mem_list == NULL)
			return FALSE;
		uint32 ui;
		for(ui = 0; ui < pool->mem_list_max_count; ui++)
			new_mem_list[ui] = pool->mem_list[ui];
		mempooll_free(pool->mem_list);
		pool->mem_list = new_mem_list;
		pool->mem_list_max_count = new_max_count;
		return mempooll_mem_list_add(pool, ptr);
	}
	return TRUE;
}

/**
	@Function:		mempooll_fill_free_list
	@Access:		Private
	@Description:
		填充MEMPOOLLPool中由index指定的MEMPOOLLList。
		index=0时，填充pool->list[0]。4Bytes的MEMPOOLLList。
		index=1时，填充pool->list[1]。8Bytes的MEMPOOLLList。
		...
		index=15时，填充pool->list[15]。8KB的MEMPOOLLList。
	@Parameters:
		pool, MEMPOOLLPoolPtr, IN OUT
			指向MEMPOOLLPool的指针。
		index, uint32, IN
			指定MEMPOOLLList的索引。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
mempooll_fill_free_list(IN OUT MEMPOOLLPoolPtr pool,
						IN uint32 index)
{
	if(pool == NULL || index >= MEMPOOLL_LIST_COUNT)
		return FALSE;
	uint32 count = KB(4) / sizeof(MEMPOOLLBlock);
	uint32 size = sizes[index];
	MEMPOOLLListPtr list = &(pool->list[index]);
	MEMPOOLLLinkedListPtr free_list = &(list->free_list);
	MEMPOOLLLinkedListPtr used_list = &(list->used_list);
	// 检测如果继续分配是否会超出配额。
	if(used_list->count * size + count * size >= MEMPOOLL_LIST_MAX_SIZE)
		return FALSE;
	
	//						Index		BlkAddr		CheckSum		Data
	uint32 size_with_head = 4 		+ 	4 		+	4			+	size;
	uint8 * blkm = mempooll_malloc(KB(4));
	if(blkm == NULL)
		return FALSE;
	if(!mempooll_mem_list_add(pool, blkm))
	{
		mempooll_free(blkm);
		return FALSE;
	}
	uint8 * mem = mempooll_malloc(count * size_with_head);
	if(mem == NULL)
	{
		mempooll_free(blkm);
		return FALSE;
	}
	if(!mempooll_mem_list_add(pool, mem))
	{
		mempooll_free(blkm);
		mempooll_free(mem);
		return FALSE;
	}
	uint32 ui;
	for(ui = 0; ui < count; ui++)
	{
		MEMPOOLLBlockPtr b = (MEMPOOLLBlockPtr)blkm + ui;
		uint8 * m = mem + ui * size_with_head;
		uint32 * indexptr = (uint32 *)(m + 0);
		uint32 * blkaddrptr = (uint32 *)(m + 4);
		uint32 * checksumptr = (uint32 *)(m + 8);
		*indexptr = index;
		*blkaddrptr = (uint32)b;
		*checksumptr = index + (uint32)b;
		b->addr = m;
		b->prev = NULL;
		b->next = NULL;
		if(!mempooll_unshift(free_list, b))
			return FALSE;
	}
	return TRUE;
}

/**
	@Function:		mempooll_get_index_by_size
	@Access:		Private
	@Description:
		获取size所对应的index。
	@Parameters:
		size, uint32, IN
			需要分配内存尺寸。
			该函数会根据这个需要分配内存尺寸返回相应的index值。
	@Return:
		uint32
			返回与size相对应的index值。
			如果返回0xffffffff则表示size过大，无法从内存池的MEMPOOLLList中获取内存块。
			需要直接调用mempooll_malloc或mempooll_calloc分配内存。
*/
static
uint32
mempooll_get_index_by_size(IN uint32 size)
{
	if(size == 0)
		return 0xffffffff;
	uint32 ui;
	for(ui = 0; ui < MEMPOOLL_LIST_COUNT; ui++)
		if(size <= sizes[ui])
			return ui;
	return 0xffffffff;
}

/**
	@Function:		mempooll_init_pool
	@Access:		Public
	@Description:
		初始化MEMPOOLLPool对象。
	@Parameters:
		pool, MEMPOOLLPoolPtr, IN OUT
			指向需要初始化的MEMPOOLLPool对象的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
mempooll_init_pool(IN OUT MEMPOOLLPoolPtr pool)
{
	if(pool == NULL)
		return FALSE;
	pool->mem_list = mempooll_malloc(MEM_LIST_INC_SIZE);
	if(pool->mem_list == NULL)
		return FALSE;
	pool->mem_list_max_count = MEM_LIST_INC_SIZE / 4;
	pool->mem_list_count = 0;
	uint32 ui;
	for(ui = 0; ui < MEMPOOLL_LIST_COUNT; ui++)
	{
		MEMPOOLLLinkedListPtr used_list = &(pool->list[ui].used_list);
		MEMPOOLLLinkedListPtr free_list = &(pool->list[ui].free_list);
		used_list->head = NULL;
		used_list->foot = NULL;
		used_list->count = 0;
		free_list->head = NULL;
		free_list->foot = NULL;
		free_list->count = 0;
	}
	return TRUE;
}

/**
	@Function:		mempooll_alloc_memory
	@Access:		Public
	@Description:
		从内存池中分配内存块。
	@Parameters:
		pool, MEMPOOLLPoolPtr, IN OUT
			指向MEMPOOLLPool对象的指针。
		size, uint32, IN
			需要分配的尺寸。
	@Return:
		void *
			成功返回内存块的指针，否则返回NULL。
*/
void *
mempooll_alloc_memory(	IN OUT MEMPOOLLPoolPtr pool,
						IN uint32 size)
{
	if(pool == NULL || size == 0)
		return NULL;
	uint32 index = mempooll_get_index_by_size(size);
	if(index == 0xffffffff)
	{
		// 如果index为0xffffffff，则表示需要向系统申请内存块而非从free链表中获取内存块。
		// 如果是向系统申请的内存块，则设置mem[0]为0xffffffff，mem[1]为申请的内存块地址，
		// mem[2]为mem[0]+mem[1]。
		//
		//						Index		BlkAddr		CheckSum		Data
		uint32 size_with_head = 4 		+ 	4 		+	4			+	size;
		uint32 * mem = mempooll_malloc(size_with_head);
		if(mem == NULL)
			return NULL;
		*(mem + 0) = 0xffffffff;
		*(mem + 1) = mem;
		*(mem + 2) = 0xffffffff + (uint32)mem;
		return mem + 3;
	}
	else
	{
		MEMPOOLLLinkedListPtr used_list = &(pool->list[index].used_list);
		MEMPOOLLLinkedListPtr free_list = &(pool->list[index].free_list);
		MEMPOOLLBlockPtr block = mempooll_shift(free_list);
		if(block == NULL)
		{
			if(!mempooll_fill_free_list(pool, index))
				return NULL;
			block = mempooll_shift(free_list);
			if(block == NULL)
			{
				// 如果无法从free链表获取内存块，则需要向系统申请内存块。可能used表的内存块数量己经到达最大值。
				// 如果是向系统申请的内存块，则设置mem[0]为0xffffffff，mem[1]为申请的内存块地址，
				// mem[2]为mem[0]+mem[1]。
				//
				//						Index		BlkAddr		CheckSum		Data
				uint32 size_with_head = 4 		+ 	4 		+	4			+	size;
				uint32 * mem = mempooll_malloc(size_with_head);
				if(mem == NULL)
					return NULL;
				*(mem + 0) = 0xffffffff;
				*(mem + 1) = mem;
				*(mem + 2) = 0xffffffff + (uint32)mem;
				return mem + 3;
			}
		}
		if(!mempooll_unshift(used_list, block))
			return NULL;
		return ((uint8 *)block->addr) + 12;
	}
}

/**
	@Function:		mempooll_free_memory
	@Access:		Public
	@Description:
		释放由内存池分配的内存块。
	@Parameters:
		pool, MEMPOOLLPoolPtr, IN OUT
			指向MEMPOOLLPool对象的指针。
		ptr, void *, IN
			指向需要释放的内存块的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
mempooll_free_memory(	IN OUT MEMPOOLLPoolPtr pool,
						IN void * ptr)
{
	if(pool == NULL || ptr == NULL)
		return FALSE;
	uint32 index = *(uint32 *)((uint8 *)ptr - 12);
	uint32 blkaddr = *(uint32 *)((uint8 *)ptr - 8);
	uint32 checksum = *(uint32 *)((uint8 *)ptr - 4);
	if(index + blkaddr != checksum)
		return FALSE;
	MEMPOOLLBlockPtr block = (MEMPOOLLBlockPtr)blkaddr;
	if(index == 0xffffffff)
	{
		mempooll_free(block);
		return TRUE;
	}
	else
	{
		MEMPOOLLLinkedListPtr used_list = &(pool->list[index].used_list);
		MEMPOOLLLinkedListPtr free_list = &(pool->list[index].free_list);
		if(!mempooll_remove(used_list, block))
			return FALSE;
		return mempooll_unshift(free_list, block);
	}
}

/**
	@Function:		mempooll_free_pool
	@Access:		Public
	@Description:
		释放内存池中的所有内存块。
	@Parameters:
		pool, MEMPOOLLPoolPtr, IN OUT
			指向MEMPOOLLPool对象的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
mempooll_free_pool(IN OUT MEMPOOLLPoolPtr pool)
{
	if(pool == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < pool->mem_list_count; ui++)
		mempooll_free(pool->mem_list[ui]);
	mempooll_free(pool->mem_list);
	return TRUE;
}

/**
	@Function:		mempooll_size
	@Access:		Public
	@Description:
		获取index对应的尺寸。
	@Parameters:
		index, uint32, IN
			对应尺寸的index。
	@Return:
		uint32
			尺寸。如果返回0则代表无法获取内存尺寸。		
*/
uint32
mempooll_size(IN uint32 index)
{
	if(index >= MEMPOOLL_LIST_COUNT)
		return 0;
	return sizes[index];
}
