#include "ilib.h"
#include "linked_list.h"
#include "string.h"
#include "Memory.h"
#include "type.h"

Memory * Memory_New(int size)
{
	Memory * memory = malloc(sizeof(Memory));
	if(memory == NULL)
	{
		printf("Memory_New: Canoot allocate memory for var memory.\n");
		return NULL;
	}
	memory->program = NULL;
	memory->Size = size;
	memory->MaxAddress = size * 2;
	memory->intdata = malloc(sizeof(cpubasetype) * size);
	if(memory->intdata == NULL)
	{
		printf("Memory_New: Canoot allocate memory for var memory->intdata.\n");
		return NULL;
	}
	memory->floatdata = malloc(sizeof(double) * size); 
	if(memory->floatdata == NULL)
	{
		printf("Memory_New: Canoot allocate memory for var memory->floatdata.\n");
		return NULL;
	}
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
