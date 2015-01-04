//Filename:		vars.h
//Author:		Ihsoh
//Date:			2014-8-27
//Descriptor:	Variables

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

extern struct Vars * alloc_vars(uint32 n);
extern BOOL free_vars(struct Vars * vars);
extern BOOL new_var(struct Vars * vars_s, int8 * name);
extern BOOL set_var_value(struct Vars * vars_s, int8 * name, int8 * value);
extern BOOL get_var_value(struct Vars * vars_s, int8 * name, int8 * value);
extern BOOL get_var_int_value(struct Vars * vars_s, int8 * name, int32 * value);
extern BOOL get_var_double_value(struct Vars * vars_s, int8 * name, double * value);
extern BOOL del_var(struct Vars * vars_s, int8 * name);

#endif
