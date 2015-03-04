#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "ilib.h"
#include "dslib/linked_list.h"
#include "type.h"
#include "InstructionInfo.h"

typedef struct
{
	int Size, MaxAddress, IntOffset, FloatOffset;
    cpubasetype * intdata;
    double * floatdata;
	InstructionInfo ** program;
} Memory;

extern Memory * Memory_New(int size);
extern void Memory_Free(Memory * memory);
extern BOOL Memory_AllocProgramSpace(Memory * memory, uint count);

#endif
