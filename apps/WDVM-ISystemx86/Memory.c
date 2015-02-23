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
	memory->program = dsl_lnklst_new();
	if(memory->program == NULL)
	{
		printf("Memory_New: Canoot allocate memory for var memory->program.\n");
		return NULL;
	}
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
	dsl_lnklst_delete_all_object_node(memory->program);
    free(memory);
}
