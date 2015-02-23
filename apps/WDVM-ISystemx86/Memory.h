#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "ilib.h"
#include "linked_list.h"
#include "type.h"
#include "InstructionInfo.h"

typedef struct
{
	int Size, MaxAddress, IntOffset, FloatOffset;
    cpubasetype * intdata;
    double * floatdata;
	DSLLinkedList * program;
} Memory;

extern Memory * Memory_New(int size);
extern void Memory_Free(Memory * memory);

#endif
