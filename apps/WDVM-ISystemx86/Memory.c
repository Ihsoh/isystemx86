#include <ilib/ilib.h>
#include <dslib/linked_list.h>
#include "Memory.h"
#include "type.h"

Memory * Memory_New(int size)
{
	Memory * memory = malloc(sizeof(Memory));
	if(memory == NULL)
		return NULL;
	memory->program = NULL;
	memory->Size = size;
	memory->MaxAddress = size * 2;
	memory->intdata = malloc(sizeof(cpubasetype) * size);
	if(memory->intdata == NULL)
		return NULL;
	memory->floatdata = malloc(sizeof(double) * size); 
	if(memory->floatdata == NULL)
		return NULL;
	memory->IntOffset = 0;
	memory->FloatOffset = size;
	return memory;
}

void Memory_Free(Memory * memory)
{
	if(memory->intdata != NULL)
		memory->intdata = NULL;
	if(memory->floatdata != NULL)
		memory->floatdata = NULL;
	if(memory->program != NULL)
		memory->program = NULL;
    free(memory);
}

BOOL Memory_AllocProgramSpace(Memory * memory, uint count)
{
	memory->program = (InstructionInfo **)malloc(sizeof(InstructionInfo *) * count);
	if(memory->program == NULL)
		return FALSE;
	return TRUE;
}
