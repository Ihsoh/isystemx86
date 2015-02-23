#include "Machine.h"
#include "ilib.h"
#include "linked_list.h"

Machine * Machine_New0(void)
{
	return malloc(sizeof(Machine));
}

Machine * Machine_New1(int memorysize)
{
	Machine * machine = malloc(sizeof(Machine));
	if(machine == NULL)
		return NULL;
	machine->memory = Memory_New(memorysize);
	machine->cpu = CPU_New(machine->memory);
	return machine;
}

Machine * Machine_New2(Memory * memory)
{
	Machine * machine = malloc(sizeof(Machine));
	if(machine == NULL)
		return NULL;
	machine->memory = memory;
	machine->cpu = CPU_New(memory);
	return machine;
}

Machine * Machine_New3(CPU * cpu, Memory * memory)
{
	Machine * machine = malloc(sizeof(Machine));
	if(machine == NULL)
		return NULL;
	machine->memory = memory;
	machine->cpu = cpu;
	return machine;
}

void Machine_Free(Machine * machine)
{
	free(machine);
}

void Machine_load0(Machine * machine, FILE * fd)
{
    int icount = bh_readint32(fd);
    int i;
    for(i = 0; i < icount; ++i)
    {
    	InstructionInfo * ii = InstructionInfo_Read(fd);
        DSLLinkedListNode * node = dsl_lnklst_new_object_node(ii);
        dsl_lnklst_add_node(machine->memory->program, node);
    }
}

BOOL Machine_load1(Machine * machine, const char * fn)
{
	FILE *fd;
    if(!(fd = fopen(fn, FILE_MODE_READ)))
    {
        return FALSE;
    }
    Machine_load0(machine, fd);
    fclose(fd);
    return TRUE;
}

void Machine_run(Machine * machine)
{
	machine->isRunning = TRUE;
	CPU_start(machine->cpu);
	while(machine->isRunning && machine->cpu->isRunning)
    {
        CPU_ClockTick(machine->cpu);
    }
}
