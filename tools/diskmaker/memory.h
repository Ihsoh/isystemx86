//Filename:		memory.c
//Author:		Ihsoh
//Date:			2014-7-24
//Descriptor:	Emulate kernel memory manager

#ifndef	_MEMORY_H_
#define	_MEMORY_H_

#include "types.h"

extern void * alloc_memory(uint length);
extern int free_memory(void * address);

#endif
