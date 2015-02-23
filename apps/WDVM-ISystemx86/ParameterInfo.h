#ifndef _PARAMETERINFO_H_
#define _PARAMETERINFO_H_

#include "ilib.h"
#include "type.h"
#include "BinaryHelper.h"

#define Param_Register 0
#define Param_Int 1
#define Param_Float 2

typedef struct
{
	int type;
    cpubasetype value;
    double value_float;
} ParameterInfo;

extern ParameterInfo ParameterInfo_Read(FILE *);

#endif
