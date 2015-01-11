//Filename:		memory.c
//Author:		Ihsoh
//Date:			2014-7-24
//Descriptor:	Emulate kernel memory manager

#include "memory.h"
#include "types.h"
#include <stdlib.h>

void * alloc_memory(uint length)
{
	return malloc(length);
}

int free_memory(void * address)
{
	free(address);
	return 1;
}
