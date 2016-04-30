/**
	@File:			vars.h
	@Author:		Ihsoh
	@Date:			2014-8-27
	@Description:
*/

#ifndef	_VARS_H_
#define	_VARS_H_

#include "types.h"

#define	MAX_VAR_NAME_BUFFER_LEN		1024
#define	MAX_VAR_NAME_LEN			(MAX_VAR_NAME_BUFFER_LEN - 1)
#define	MAX_VAR_VALUE_BUFFER_LEN	1024
#define	MAX_VAR_VALUE_LEN			(MAX_VAR_VALUE_BUFFER_LEN - 1)

struct Var
{
	int32	used;
	int8	name[MAX_VAR_NAME_BUFFER_LEN];
	int8	value[MAX_VAR_VALUE_BUFFER_LEN];
};

struct Vars
{
	uint32			max_var_count;
	struct Var *	vars;
};

extern
struct Vars *
KnlAllocVarList(IN uint32 n);

extern
BOOL
KnlFreeVarList(IN struct Vars * vars);

extern
BOOL
KnlNewVar(	IN struct Vars * vars_s,
			IN int8 * name);

extern
BOOL
KnlSetVarValue(	IN OUT struct Vars * vars_s,
				IN int8 * name,
				IN int8 * value);

extern
BOOL
KnlGetVarValue(	IN struct Vars * vars_s,
				IN int8 * name,
				OUT int8 * value);

extern
BOOL
KnlGetVarValueWithSize(	IN struct Vars * vars_s,
						IN int8 * name,
						OUT int8 * value,
						IN uint32 size);

extern
BOOL
KnlGetVarIntValue(	IN struct Vars * vars_s,
					IN int8 * name,
					OUT int32 * value);

extern
BOOL
KnlGetVarDoubleValue(	IN struct Vars * vars_s,
						IN int8 * name,
						OUT double * value);

extern
BOOL
KnlDeleteVar(	IN OUT struct Vars * vars_s,
				IN int8 * name);

#endif
