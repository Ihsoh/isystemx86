#ifndef _CPU_H_
#define _CPU_H_

#include <ilib/ilib.h>
#include "type.h"
#include "Memory.h"
#include "InterruptInfo.h"
#include "InstructionInfo.h"

#define InterruptCount 256

typedef struct
{
    BOOL InterruptEnable, isRunning;
    InterruptInfo InterruptVector[InterruptCount];
    int RanCount, InterruptId;
    cpubasetype eax, ebx, ecx, edx, ebp, esp, ebfp, esfp, eip;
    cpubasetype r0, r1, r2, r3, r4, r5, r6, r7;
    double f0, f1, f2, f3, f4, f5, f6, f7;
    Memory * memory;
} CPU;

extern CPU * CPU_New(Memory * memory);
extern void CPU_Free(CPU * cpu);
extern void CPU_start(CPU * cpu);
extern void CPU_halt(CPU * cpu);
extern void CPU_ClockTick(CPU * cpu);
extern void CPU_execute(CPU * cpu);
extern cpubasetype CPU_getIntValue(CPU * cpu, ParameterInfo * pi);
extern double CPU_getFloatValue(CPU * cpu, ParameterInfo * pi);
extern void CPU_pushi(CPU * cpu, cpubasetype num);
extern cpubasetype CPU_popi(CPU * cpu);
extern void CPU_pushf(CPU * cpu, double num);
extern double CPU_popf(CPU * cpu);
extern void CPU_pushreg(CPU * cpu);
extern void CPU_popreg(CPU * cpu);
extern void CPU_AddInterrupt(CPU * cpu, int id, pInterruptCallback pic);
extern void CPU_Interrupt(CPU * cpu, int id);

extern void CPU_executeInstruction(CPU * cpu, InstructionInfo * ii);

extern cpubasetype CPU_getIntRegisterValue(CPU * cpu, cpubasetype id);
extern void CPU_setIntRegisterValue(CPU * cpu, cpubasetype id, cpubasetype data);
extern double CPU_getFloatRegisterValue(CPU * cpu, cpubasetype id);
extern void CPU_setFloatRegisterValue(CPU * cpu, cpubasetype id, double data);

#endif
