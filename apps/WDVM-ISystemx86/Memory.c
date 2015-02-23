#include "ilib.h"
#include "linked_list.h"
#include "string.h"
#include "Memory.h"
#include "type.h"

Memory * Memory_New(int size)
{
	Memory * memory = malloc(sizeof(Memory));
	if(memory == NULL)
		return NULL;
	memory->program = dsl_lnklst_new();
	memory->Size = size;
	memory->MaxAddress = size * 2;
	memory->intdata = malloc(sizeof(cpubasetype) * size);
	memory->floatdata = malloc(sizeof(double) * size); 
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
