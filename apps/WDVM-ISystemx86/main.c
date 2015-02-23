#include "ilib.h"
#include "dslib.h"

#include "Machine.h"
#include "BinaryHelper.h"

void OnInterrupt(void * obj)
{
    CPU * cpu= (CPU *)obj;
    switch(cpu->InterruptId)
    {
        case 5:
            print_char(cpu->eax);
            break;
        case 9:
            cpu->eax = get_char();
            break;
        case 13:
            //TODO
            break;
        default:
            break;
    }
}

int main(int argc, char * argv[])
{
	if(argc != 2)
	{
		printf("wdvm {file name}\n");
		return -1;
	}

	DSLEnvironment env;
	env.dsl_malloc = malloc;
	env.dsl_calloc = calloc;
	env.dsl_free = free;
	dsl_init(&env);

	Machine * machine = Machine_New1(/*1 * 1024 * */ 512);

	if(machine == NULL)
	{
		printf("Cannot create machine\n");
        return -1;
	}

	if(!Machine_load1(machine, argv[1]))
    {
        printf("Cannot open %s\n", argv[1]);
        return -1;
    }

	CPU_AddInterrupt(machine->cpu, 5, OnInterrupt);
	CPU_AddInterrupt(machine->cpu, 9, OnInterrupt);
	CPU_AddInterrupt(machine->cpu, 13, OnInterrupt);

	printf("Ready...\n");

	Machine_run(machine);

	return 0;
}
