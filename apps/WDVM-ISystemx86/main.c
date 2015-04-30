#include <ilib/ilib.h>
#include <dslib/dslib.h>

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

	Machine * machine = Machine_New1(2 * 1024);

	if(machine == NULL)
	{
		printf("Cannot create machine\n");
        return -1;
	}

	char file_path[1024];
	strcpy(file_path, argv[1]);
	fix_path(file_path, file_path);

	if(!Machine_load1(machine, file_path))
    {
        printf("Cannot open %s\n", argv[1]);
        return -1;
    }

	CPU_AddInterrupt(machine->cpu, 5, OnInterrupt);
	CPU_AddInterrupt(machine->cpu, 9, OnInterrupt);
	CPU_AddInterrupt(machine->cpu, 13, OnInterrupt);

	Machine_run(machine);

	return 0;
}
