/**
	@File:			mempoollib.h
	@Author:		Ihsoh
	@Date:			2015-05-08
	@Description:
		内存池。
*/

#ifndef	_MEMPOOLLIB_H_
#define	_MEMPOOLLIB_H_

#include "types.h"

typedef void * (* MEMPOOLLMalloc)(IN uint32 num_bytes);
typedef void * (* MEMPOOLLCalloc)(IN uint32 n, IN uint32 size);
typedef void (* MEMPOOLLFree)(IN void * ptr);

typedef struct _MEMPOOLLEnvironment
{
	MEMPOOLLMalloc	mempooll_malloc;
	MEMPOOLLCalloc	mempooll_calloc;
	MEMPOOLLFree	mempooll_free;
} MEMPOOLLEnvironment, * MEMPOOLLEnvironmentPtr;

extern MEMPOOLLMalloc	mempooll_malloc;
extern MEMPOOLLCalloc	mempooll_calloc;
extern MEMPOOLLFree		mempooll_free;

typedef struct _MEMPOOLLBlock
{
	void *					addr;
	struct _MEMPOOLLBlock *	prev;
	struct _MEMPOOLLBlock *	next;
} MEMPOOLLBlock, * MEMPOOLLBlockPtr;

typedef struct _MEMPOOLLLinkedList
{
	MEMPOOLLBlockPtr	head;
	MEMPOOLLBlockPtr	foot;
	uint32				count;
} MEMPOOLLLinkedList, * MEMPOOLLLinkedListPtr;

typedef struct _MEMPOOLLList
{
	MEMPOOLLLinkedList	used_list;
	MEMPOOLLLinkedList	free_list;
} MEMPOOLLList, * MEMPOOLLListPtr;

#define	MEMPOOLL_LIST_COUNT			16			// 内存池的内存块列表数。
#define	MEMPOOLL_LIST_MAX_SIZE		(MB(8))		// 每张内存块列表的内存块总大小。

typedef struct _MEMPOOLLPool
{
	MEMPOOLLList 	list[MEMPOOLL_LIST_COUNT];
	void **				mem_list;
	uint32				mem_list_max_count;
	uint32				mem_list_count;
} MEMPOOLLPool, * MEMPOOLLPoolPtr;

extern
BOOL
mempooll_init(IN MEMPOOLLEnvironmentPtr env);

extern
BOOL
mempooll_init_pool(IN OUT MEMPOOLLPoolPtr pool);

extern
void *
mempooll_alloc_memory(	IN OUT MEMPOOLLPoolPtr pool,
						IN uint32 size);

extern
BOOL
mempooll_free_memory(	IN OUT MEMPOOLLPoolPtr pool,
						IN void * ptr);

extern
BOOL
mempooll_free_pool(IN OUT MEMPOOLLPoolPtr pool);

extern
uint32
mempooll_size(IN uint32 index);

#endif
