/**
	@File:			vars.c
	@Author:		Ihsoh
	@Date:			2014-8-27
	@Description:
		提供系统变量的功能。
*/

#include "vars.h"
#include "types.h"
#include "memory.h"
#include <ilib/string.h>

/**
	@Function:		alloc_vars
	@Access:		Public
	@Description:
		创建变量列表。
	@Parameters:
		n, uint32, IN
			最大变量数。
	@Return:
		struct Vars *
			指向分配的变量列表的指针。		
*/
struct Vars *
alloc_vars(IN uint32 n)
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

/**
	@Function:		free_vars
	@Access:		Public
	@Description:
		释放变量列表。
	@Parameters:
		vars_s, struct Vars *, IN
			指向分配的变量列表的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
free_vars(IN struct Vars * vars_s)
{
	if(free_memory(vars_s->vars) && free_memory(vars_s))
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		is_valid_var_name
	@Access:		Public
	@Description:
		检查变量名是否合法。
	@Parameters:
		name, int8 *, IN
			变量名。
	@Return:
		BOOL
			返回TRUE则合法，否则不合法。		
*/
static
BOOL
is_valid_var_name(IN int8 * name)
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

/**
	@Function:		get_var
	@Access:		Public
	@Description:
		获取变量。
	@Parameters:
		vars_s, struct Vars *, IN
			指向分配的变量列表的指针。
		name, int8 *, IN
			变量名。
	@Return:
		struct Var *
			指向变量的结构体的指针。		
*/
static
struct Var *
get_var(IN struct Vars * vars_s,
		IN int8 * name)
{
	uint32 ui;
	struct Var * vars = vars_s->vars;
	for(ui = 0; ui < vars_s->max_var_count; ui++)
		if(vars[ui].used && strcmp(vars[ui].name, name) == 0)
			return vars + ui;
	return NULL;
}

/**
	@Function:		var_is_exists
	@Access:		Private
	@Description:
		获取变量。
	@Parameters:
		vars_s, struct Vars *, IN
			指向分配的变量列表的指针。
		name, int8 *, IN
			变量名。
	@Return:
		BOOL
			返回TRUE则存在，否则不存在。		
*/
static
BOOL
var_is_exists(	IN struct Vars * vars_s,
				IN int8 * name)
{
	if(get_var(vars_s, name) != NULL)
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		new_var
	@Access:		Public
	@Description:
		新建变量。
	@Parameters:
		vars_s, struct Vars *, IN OUT
			指向分配的变量列表的指针。
		name, int8 *, IN
			变量名。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
new_var(IN OUT struct Vars * vars_s,
		IN int8 * name)
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

/**
	@Function:		set_var_value
	@Access:		Public
	@Description:
		设置变量的值。
	@Parameters:
		vars_s, struct Vars *, IN OUT
			指向分配的变量列表的指针。
		name, int8 *, IN
			变量名。
		value, int8 *, IN
			值。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
set_var_value(	IN OUT struct Vars * vars_s,
				IN int8 * name,
				IN int8 * value)
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

/**
	@Function:		get_var_value
	@Access:		Public
	@Description:
		获取变量的值。
	@Parameters:
		vars_s, struct Vars *, IN
			指向分配的变量列表的指针。
		name, int8 *, IN
			变量名。
		value, int8 *, OUT
			值。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
get_var_value(	IN struct Vars * vars_s,
				IN int8 * name,
				OUT int8 * value)
{
	return get_var_value_with_size(vars_s, name, value, 0x7fffffff);
}

/**
	@Function:		get_var_value_with_size
	@Access:		Public
	@Description:
		获取变量的值，并且可以指定储存值的缓冲区的大小。
	@Parameters:
		vars_s, struct Vars *, IN
			指向分配的变量列表的指针。
		name, int8 *, IN
			变量名。
		value, int8 *, OUT
			值。
		size, uint32, IN
			储存值的缓冲区的大小。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
get_var_value_with_size(IN struct Vars * vars_s,
						IN int8 * name,
						OUT int8 * value,
						IN uint32 size)
{
	if(	vars_s == NULL 
		|| size == 0
		|| !is_valid_var_name(name)
		|| !var_is_exists(vars_s, name))
		return FALSE;
	struct Var * var = get_var(vars_s, name);
	if(var == NULL)
		return FALSE;
	char chr;
	int32 index = 0;
	size--;	//'\0'
	while(size-- != 0 && (chr = var->value[index++]) != '\0')
		*(value++) = chr;
	*value = '\0';
	return TRUE;
}

/**
	@Function:		get_var_int_value
	@Access:		Public
	@Description:
		获取变量的值。
	@Parameters:
		vars_s, struct Vars *, IN
			指向分配的变量列表的指针。
		name, int8 *, IN
			变量名。
		value, int32 *, OUT
			值。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
get_var_int_value(	IN struct Vars * vars_s,
					IN int8 * name,
					OUT int32 * value)
{
	int8 buffer[MAX_VAR_VALUE_BUFFER_LEN];
	if(!get_var_value(vars_s, name, buffer))
		return FALSE;
	*value = (int32)stol(buffer);
	return TRUE;
}

/**
	@Function:		get_var_double_value
	@Access:		Public
	@Description:
		获取变量的值。
	@Parameters:
		vars_s, struct Vars *, IN
			指向分配的变量列表的指针。
		name, int8 *, IN
			变量名。
		value, double *, OUT
			值。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
get_var_double_value(	IN struct Vars * vars_s,
						IN int8 * name,
						OUT double * value)
{
	int8 buffer[MAX_VAR_VALUE_BUFFER_LEN];
	if(!get_var_value(vars_s, name, buffer))
		return FALSE;
	*value = stod(buffer);
	return TRUE;
}

/**
	@Function:		del_var
	@Access:		Public
	@Description:
		删除变量。
	@Parameters:
		vars_s, struct Vars *, IN OUT
			指向分配的变量列表的指针。
		name, int8 *, IN
			变量名。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
del_var(IN OUT struct Vars * vars_s,
		IN int8 * name)
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
