#ifndef _MACHINE_H_
#define _MACHINE_H_

#include <ilib/ilib.h>
#include "type.h"
#include "CPU.h"
#include "Memory.h"

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#define abs(a) ((a)<0?(-(a)):(a))

typedef struct
{
    BOOL isRunning;
    int DebugLevel;
    Memory * memory;
    CPU * cpu;
    ILFILE * hdd;
} Machine;

extern Machine * Machine_New0(void);
extern Machine * Machine_New1(int);
extern Machine * Machine_New2(Memory *);
extern Machine * Machine_New3(CPU *, Memory *);
extern void Machine_Free(Machine *);
extern void Machine_load0(Machine *, ILFILE *);
extern BOOL Machine_load1(Machine *, const char *);
extern void Machine_run(Machine *);

#endif
