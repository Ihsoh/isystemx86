#ifndef _INSTRUCTIONINFO_H_
#define _INSTRUCTIONINFO_H_

#include "ilib.h"
#include "linked_list.h"
#include "type.h"
#include "BinaryHelper.h"
#include "ParameterInfo.h"

typedef struct
{
	unsigned short ID;
    char pcount;
    DSLLinkedList * pi;
} InstructionInfo;

extern InstructionInfo * InstructionInfo_New(void);
extern void InstructionInfo_Free(InstructionInfo * ii);
extern InstructionInfo * InstructionInfo_Read(FILE *);

#endif
