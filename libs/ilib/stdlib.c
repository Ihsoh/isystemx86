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
#include "string.h"

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

void * realloc(void * mem_address, unsigned int newsize)
{
	if(mem_address == NULL)
		return NULL;
	uint32 * addr = mem_address;
	uint32 index = *(addr - 3);
	uint32 blkaddr = *(addr - 2);
	uint32 checksum = *(addr - 1);
	if(index + blkaddr != checksum)
		return NULL;
	MEMPOOLLBlockPtr blk = (MEMPOOLLBlockPtr)blkaddr;
	if(blk == NULL)
		return NULL;
	uint32 oldsize = mempooll_size(index);
	if(oldsize == 0)
		oldsize = il_memory_size(blk);
	if(oldsize == 0)
		return NULL;
	if(newsize <= oldsize)
		return mem_address;
	void * newmem = malloc(newsize);
	if(newmem == NULL)
		return NULL;
	memcpy(newmem, mem_address, oldsize);
	free(mem_address);
	return newmem;
}

void exit(int status)
{
	__set_retvalue(status);
	app_exit();
}

void swap(void *a, void *b, int size)
{
    unsigned char *bytesOfA = (uint8 *)a;
    unsigned char *bytesOfB = (uint8 *)b;
    char tmp = 0;
    int i;
    for(i = 0; i < size; ++i)
    {
        tmp = *(bytesOfA+i);
        *(bytesOfA+i) = *(bytesOfB+i);
        *(bytesOfB+i) = tmp;
    }
}

int partition(void *base, int num, int size, int (*comparator)(const void *, const void *))
{
    int first = 0, end = num-1;
    while(first < end)
    {
        while(first < end && comparator(base+first*size, base+end*size) <= 0)
        {
            --end;
        }
        if(first < end)
        {
            swap(base+first*size, base+end*size, size);
            ++first;
        }
        while(first < end && comparator(base+first*size, base+end*size) <= 0)
        {
            ++first;
        }
        if(first < end)
        {
            swap(base+first*size, base+end*size, size);
            --end;
        }
    }
    return first;
}

void qsort(void *base, int num, int size, int (*comparator)(const void *, const void *))
{
    if(num > 0)
    {
        int pivot = partition(base, num, size, comparator);
        qsort(base, pivot, size, comparator);
        qsort(base+(pivot+1)*size, num-pivot-1, size, comparator);
    }
}
