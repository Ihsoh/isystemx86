#include "Machine.h"
#include "ilib.h"
#include "dslib/linked_list.h"

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
	if(machine->memory == NULL)
	{
		printf("Machine_New1: Memory_New return NULL.\n");
		return NULL;
	}
	machine->cpu = CPU_New(machine->memory);
	if(machine->cpu == NULL)
	{
		printf("Machine_New1: CPU_New return NULL.\n");
		return NULL;
	}
	return machine;
}

Machine * Machine_New2(Memory * memory)
{
	Machine * machine = malloc(sizeof(Machine));
	if(machine == NULL)
		return NULL;
	machine->memory = memory;
	if(machine->memory == NULL)
		return NULL;
	machine->cpu = CPU_New(memory);
	if(machine->cpu == NULL)
	{
		printf("Machine_New1: CPU_New return NULL.\n");
		return NULL;
	}
	return machine;
}

Machine * Machine_New3(CPU * cpu, Memory * memory)
{
	Machine * machine = malloc(sizeof(Machine));
	if(machine == NULL)
		return NULL;
	machine->memory = memory;
	if(machine->memory == NULL)
		return NULL;
	machine->cpu = cpu;
	if(machine->cpu == NULL)
		return NULL;
	return machine;
}

void Machine_Free(Machine * machine)
{
	free(machine);
}

void Machine_load0(Machine * machine, FILE * fd)
{
    int icount = bh_readint32(fd);
    if(!Memory_AllocProgramSpace(machine->memory, icount))
    {
    	printf("Machine_load0: Cannot allocate memory for instructions!\n");
    	app_exit();
    }
    int i;
    for(i = 0; i < icount; ++i)
    {
    	InstructionInfo * ii = InstructionInfo_Read(fd);
    	if(ii == NULL)
    	{
    		printf("Machine_load0: ii is NULL!\n");
    		app_exit();
    	}
        machine->memory->program[i] = ii;
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
