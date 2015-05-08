/**
	@File:			stdlib.c
	@Author:		Ihsoh
	@Date:			2014-1-19
	@Description:
		提供内存分配、释放等杂项功能。
*/

#include "stdlib.h"
#include "types.h"
#include "system.h"

#include <mempoollib/mempoollib.h>

static MEMPOOLLPool pool;
static BOOL pool_was_inited = FALSE;

static void * __malloc(uint num_bytes)
{
	return il_allocm(num_bytes);
}

static void * __calloc(uint n, uint size)
{
	return il_allocm(n * size);
}

static void __free(void * ptr)
{
	il_freem(ptr);
}

BOOL __init_mempool(void)
{
	if(!pool_was_inited)
	{
		MEMPOOLLEnvironment mpoolenv;
		mpoolenv.mempooll_malloc = __malloc;
		mpoolenv.mempooll_calloc = __calloc;
		mpoolenv.mempooll_free = __free;
		pool_was_inited = mempooll_init(&mpoolenv);
	}
	return pool_was_inited;
}

BOOL __destroy_mempool(void)
{
	return mempooll_free_pool(&pool);
}

void * malloc(uint num_bytes)
{
	return mempooll_alloc_memory(&pool, num_bytes);
}

void * calloc(uint n, uint size)
{
	return mempooll_alloc_memory(&pool, n * size);
}

void free(void * ptr)
{
	mempooll_free_memory(&pool, ptr);
}
