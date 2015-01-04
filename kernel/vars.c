//Filename:		vars.c
//Author:		Ihsoh
//Date:			2014-8-27
//Descriptor:	Variables

#include "vars.h"
#include "types.h"
#include "memory.h"
#include <string.h>

struct Vars *
alloc_vars(uint32 n)
{
	if(n == 0)
		return NULL;
	struct Vars * vars_s = (struct Vars *)alloc_memory(sizeof(struct Vars));
	if(vars_s == NULL)
		return NULL;
	vars_s->vars = alloc_memory(n * sizeof(struct Var));
	if(vars_s->vars == NULL)
		return NULL;
	uint32 ui;
	for(ui = 0; ui < n; ui++)
		vars_s->vars[ui].used = 0;
	vars_s->max_var_count = n;
	return vars_s;
}

BOOL
free_vars(struct Vars * vars_s)
{
	if(free_memory(vars_s->vars) && free_memory(vars_s))
		return TRUE;
	else
		return FALSE;
}

static
BOOL
is_valid_var_name(int8 * name)
{
	if(name == NULL)
		return FALSE;
	uint32 len = strlen(name);
	if(len == 0 || len >= MAX_VAR_NAME_LEN)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < len; ui++)
	{
		int8 chr = name[ui];
		if(!((chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z') || (chr >= '0' && chr <= '9') || chr == '_'))
			return FALSE;
	}
	return TRUE;
}

static
struct Var *
get_var(struct Vars * vars_s, int8 * name)
{
	uint32 ui;
	struct Var * vars = vars_s->vars;
	for(ui = 0; ui < vars_s->max_var_count; ui++)
		if(vars[ui].used && strcmp(vars[ui].name, name) == 0)
			return vars + ui;
	return NULL;
}

static
BOOL
var_is_exists(struct Vars * vars_s, int8 * name)
{
	if(get_var(vars_s, name) != NULL)
		return TRUE;
	else
		return FALSE;
}

BOOL
new_var(struct Vars * vars_s, int8 * name)
{
	if(	vars_s == NULL 
		|| !is_valid_var_name(name) 
		|| var_is_exists(vars_s, name))
		return FALSE;
	struct Var * vars = vars_s->vars;
	uint32 index;
	for(index = 0; index < vars_s->max_var_count && vars[index].used != 0; index++);
	if(index == vars_s->max_var_count)
		return FALSE;
	vars[index].used = 1;
	strcpy(vars[index].name, name);
	vars[index].value[0] = '\0';
	return TRUE;
}

BOOL
set_var_value(struct Vars * vars_s, int8 * name, int8 * value)
{
	if(	vars_s == NULL 
		|| !is_valid_var_name(name)
		|| value == NULL 
		|| strlen(value) > MAX_VAR_VALUE_LEN
		|| !var_is_exists(vars_s, name))
		return FALSE;
	struct Var * var = get_var(vars_s, name);
	if(var == NULL)
		return FALSE;
	strcpy(var->value, value);
	return TRUE;
}

BOOL
get_var_value(struct Vars * vars_s, int8 * name, int8 * value)
{
	if(	vars_s == NULL 
		|| !is_valid_var_name(name)
		|| !var_is_exists(vars_s, name))
		return FALSE;
	struct Var * var = get_var(vars_s, name);
	if(var == NULL)
		return FALSE;
	strcpy(value, var->value);
	return TRUE;
}

BOOL
get_var_int_value(struct Vars * vars_s, int8 * name, int32 * value)
{
	int8 buffer[MAX_VAR_VALUE_BUFFER_LEN];
	if(!get_var_value(vars_s, name, buffer))
		return FALSE;
	*value = (int32)stol(buffer);
	return TRUE;
}

BOOL
get_var_double_value(struct Vars * vars_s, int8 * name, double * value)
{
	int8 buffer[MAX_VAR_VALUE_BUFFER_LEN];
	if(!get_var_value(vars_s, name, buffer))
		return FALSE;
	*value = stod(buffer);
	return TRUE;
}

BOOL
del_var(struct Vars * vars_s, int8 * name)
{
	if(	vars_s == NULL 
		|| !is_valid_var_name(name)
		|| !var_is_exists(vars_s, name))
		return FALSE;
	struct Var * var = get_var(vars_s, name);
	if(var == NULL)
		return FALSE;
	var->used = 0;
	return TRUE;
}
