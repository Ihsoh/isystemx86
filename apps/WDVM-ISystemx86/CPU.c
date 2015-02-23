#include "ilib.h"
#include "CPU.h"

CPU * CPU_New(Memory * memory)
{
	CPU * cpu = malloc(sizeof(CPU));
	if(cpu == NULL)
		return NULL;
	cpu->eax = cpu->ebx = cpu->ecx = cpu->edx = cpu->ebp = cpu->esp = cpu->ebfp = cpu->esfp = cpu->eip=0;
    cpu->r0 = cpu->r1 = cpu->r2 = cpu->r3 = cpu->r4 = cpu->r5 = cpu->r6 = cpu->r7 = 0;
    cpu->f0 = cpu->f1 = cpu->f2 = cpu->f3 = cpu->f4 = cpu->f5 = cpu->f6 = cpu->f7 = 0.0;
    int i;
    for(i = 0; i < InterruptCount; ++i)
    {
        cpu->InterruptVector[i].type = Interrupt_Outer;
        cpu->InterruptVector[i].outer_callback = NULL;
    }
    cpu->memory = memory;
    return cpu;
}

void CPU_Free(CPU * cpu)
{
	free(cpu);
}

void CPU_start(CPU * cpu)
{
	cpu->isRunning = TRUE;
}

void CPU_halt(CPU * cpu)
{
	cpu->isRunning = FALSE;
}

void CPU_ClockTick(CPU * cpu)
{
	if(cpu->isRunning)
		CPU_execute(cpu);
}

void CPU_execute(CPU * cpu)
{
    /*printf( "CPU Execute cpu: %x, cpu->eip: %d, cpu->memory: %x, cpu->memory->program: %x\n", 
            cpu,
            cpu->eip, 
            cpu->memory,
            cpu->memory->program);*/


    DSLLinkedListNode * node = dsl_lnklst_get(cpu->memory->program, cpu->eip);
    //printf("CPU Execute Node: %x\n", node);
	InstructionInfo * ii = node->value.value.object_value;
    printf("CPU Excutue, OPCODE: %d, PCOUNT: %d\n", ii->ID, ii->pcount);
    if(ii->pcount != 0)
    {
        uint ui;
        for(ui = 0; ui < ii->pcount; ui++)
        {
            DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, ui);
            ParameterInfo * pi = node->value.value.object_value;
            printf("type: %d, basevalue: %d, floatvalue: %f\n", pi->type, pi->value, pi->value_float);
        }
    }
	++(cpu->eip);
	CPU_executeInstruction(cpu, ii);
	++(cpu->RanCount);
    //printf("CPU Executed EIP: %d\n", cpu->eip);
}

cpubasetype CPU_getIntValue(CPU * cpu, ParameterInfo * pi)
{
	if(pi->type == Param_Int)
		return pi->value;
	else
		return CPU_getIntRegisterValue(cpu, pi->value);
}

double CPU_getFloatValue(CPU * cpu, ParameterInfo * pi)
{
	if(pi->type == Param_Float)
		return pi->value_float;
	else
		return CPU_getFloatRegisterValue(cpu, pi->value);
}

void CPU_pushi(CPU * cpu, cpubasetype num)
{
	++(cpu->esp);
	cpu->memory->intdata[cpu->esp] = num;
}

cpubasetype CPU_popi(CPU * cpu)
{
	cpubasetype num;
    num = cpu->memory->intdata[cpu->esp];
    --(cpu->esp);
    return num;
}

void CPU_pushf(CPU * cpu, double num)
{
	++(cpu->esfp);
	cpu->memory->floatdata[cpu->esp] = num;
}

double CPU_popf(CPU * cpu)
{
	double num;
    num = cpu->memory->floatdata[cpu->esfp];
    --(cpu->esfp);
    return num;
}

void CPU_pushreg(CPU * cpu)
{
	CPU_pushi(cpu, cpu->eax);
    CPU_pushi(cpu, cpu->ebx);
    CPU_pushi(cpu, cpu->ecx);
    CPU_pushi(cpu, cpu->edx);
    CPU_pushi(cpu, cpu->esp);
    CPU_pushi(cpu, cpu->esfp);
    CPU_pushi(cpu, cpu->eip);
    CPU_pushi(cpu, cpu->r0);
    CPU_pushi(cpu, cpu->r1);
    CPU_pushi(cpu, cpu->r2);
    CPU_pushi(cpu, cpu->r3);
    CPU_pushi(cpu, cpu->r4);
    CPU_pushi(cpu, cpu->r5);
    CPU_pushi(cpu, cpu->r6);
    CPU_pushi(cpu, cpu->r7);
    CPU_pushf(cpu, cpu->f0);
    CPU_pushf(cpu, cpu->f1);
    CPU_pushf(cpu, cpu->f2);
    CPU_pushf(cpu, cpu->f3);
    CPU_pushf(cpu, cpu->f4);
    CPU_pushf(cpu, cpu->f5);
    CPU_pushf(cpu, cpu->f6);
    CPU_pushf(cpu, cpu->f7);
}

void CPU_popreg(CPU * cpu)
{
	cpu->f7 = CPU_popf(cpu);
    cpu->f6 = CPU_popf(cpu);
    cpu->f5 = CPU_popf(cpu);
    cpu->f4 = CPU_popf(cpu);
    cpu->f3 = CPU_popf(cpu);
    cpu->f2 = CPU_popf(cpu);
    cpu->f1 = CPU_popf(cpu);
    cpu->f0 = CPU_popf(cpu);
    cpu->r7 = CPU_popi(cpu);
    cpu->r6 = CPU_popi(cpu);
    cpu->r5 = CPU_popi(cpu);
    cpu->r4 = CPU_popi(cpu);
    cpu->r3 = CPU_popi(cpu);
    cpu->r2 = CPU_popi(cpu);
    cpu->r1 = CPU_popi(cpu);
    cpu->r0 = CPU_popi(cpu);
    cpu->eip = CPU_popi(cpu);
    cpu->esfp = CPU_popi(cpu);
    cpu->esp = CPU_popi(cpu);
    cpu->edx = CPU_popi(cpu);
    cpu->ecx = CPU_popi(cpu);
    cpu->ebx = CPU_popi(cpu);
    cpu->eax = CPU_popi(cpu);
}

void CPU_AddInterrupt(CPU * cpu, int id, pInterruptCallback pic)
{
	InterruptInfo * ii = cpu->InterruptVector + id;
    ii->type = Interrupt_Outer;
    ii->outer_callback = pic;
}

void CPU_Interrupt(CPU * cpu, int id)
{
	if (!cpu->InterruptEnable) 
		return;
    cpu->InterruptId = id;
    InterruptInfo * ii = cpu->InterruptVector + id;
    if (ii->type == Interrupt_Inner)
    {
        CPU_pushi(cpu, cpu->eip);
        cpu->eip = ii->inner_address;
    }
    else
    {
        if (ii->outer_callback != NULL)
        	ii->outer_callback(cpu);
    }
}
