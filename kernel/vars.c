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
	@Function:		KnlAllocVarList
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
KnlAllocVarList(IN uint32 n)
{
	if(n == 0)
		return NULL;
	struct Vars * vars_s = (struct Vars *)MemAlloc(sizeof(struct Vars));
	if(vars_s == NULL)
		return NULL;
	vars_s->vars = MemAlloc(n * sizeof(struct Var));
	if(vars_s->vars == NULL)
		return NULL;
	uint32 ui;
	for(ui = 0; ui < n; ui++)
		vars_s->vars[ui].used = 0;
	vars_s->max_var_count = n;
	return vars_s;
}

/**
	@Function:		KnlFreeVarList
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
KnlFreeVarList(IN struct Vars * vars_s)
{
	if(MemFree(vars_s->vars) && MemFree(vars_s))
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		_KnlIsValidVarName
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
_KnlIsValidVarName(IN int8 * name)
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
		if(!(	(chr >= 'A' && chr <= 'Z')
				|| (chr >= 'a' && chr <= 'z')
				|| (chr >= '0' && chr <= '9')
				|| chr == '_'))
			return FALSE;
	}
	return TRUE;
}

/**
	@Function:		_KnlGetVar
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
_KnlGetVar(	IN struct Vars * vars_s,
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
	@Function:		_KnlHasVar
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
_KnlHasVar(	IN struct Vars * vars_s,
			IN int8 * name)
{
	if(_KnlGetVar(vars_s, name) != NULL)
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		KnlNewVar
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
KnlNewVar(	IN OUT struct Vars * vars_s,
			IN int8 * name)
{
	if(	vars_s == NULL 
		|| !_KnlIsValidVarName(name) 
		|| _KnlHasVar(vars_s, name))
		return FALSE;
	struct Var * vars = vars_s->vars;
	uint32 index;
	for(index = 0; index < vars_s->max_var_count && vars[index].used != 0; index++);
	if(index == vars_s->max_var_count)
		return FALSE;
	vars[index].used = 1;
	UtlCopyString(vars[index].name, sizeof(vars[index].name), name);
	vars[index].value[0] = '\0';
	return TRUE;
}

/**
	@Function:		KnlSetVarValue
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
KnlSetVarValue(	IN OUT struct Vars * vars_s,
				IN int8 * name,
				IN int8 * value)
{
	if(	vars_s == NULL 
		|| !_KnlIsValidVarName(name)
		|| value == NULL 
		|| strlen(value) > MAX_VAR_VALUE_LEN
		|| !_KnlHasVar(vars_s, name))
		return FALSE;
	struct Var * var = _KnlGetVar(vars_s, name);
	if(var == NULL)
		return FALSE;
	UtlCopyString(var->value, sizeof(var->value), value);
	return TRUE;
}

/**
	@Function:		KnlGetVarValue
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
KnlGetVarValue(	IN struct Vars * vars_s,
				IN int8 * name,
				OUT int8 * value)
{
	return KnlGetVarValueWithSize(vars_s, name, value, 0x7fffffff);
}

/**
	@Function:		KnlGetVarValueWithSize
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
KnlGetVarValueWithSize(	IN struct Vars * vars_s,
						IN int8 * name,
						OUT int8 * value,
						IN uint32 size)
{
	if(	vars_s == NULL 
		|| size == 0
		|| !_KnlIsValidVarName(name)
		|| !_KnlHasVar(vars_s, name))
		return FALSE;
	struct Var * var = _KnlGetVar(vars_s, name);
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
	@Function:		KnlGetVarIntValue
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
KnlGetVarIntValue(	IN struct Vars * vars_s,
					IN int8 * name,
					OUT int32 * value)
{
	int8 buffer[MAX_VAR_VALUE_BUFFER_LEN];
	if(!KnlGetVarValue(vars_s, name, buffer))
		return FALSE;
	*value = (int32)stol(buffer);
	return TRUE;
}

/**
	@Function:		KnlGetVarDoubleValue
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
KnlGetVarDoubleValue(	IN struct Vars * vars_s,
						IN int8 * name,
						OUT double * value)
{
	int8 buffer[MAX_VAR_VALUE_BUFFER_LEN];
	if(!KnlGetVarValue(vars_s, name, buffer))
		return FALSE;
	*value = stod(buffer);
	return TRUE;
}

/**
	@Function:		KnlDeleteVar
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
KnlDeleteVar(	IN OUT struct Vars * vars_s,
				IN int8 * name)
{
	if(	vars_s == NULL 
		|| !_KnlIsValidVarName(name)
		|| !_KnlHasVar(vars_s, name))
		return FALSE;
	struct Var * var = _KnlGetVar(vars_s, name);
	if(var == NULL)
		return FALSE;
	var->used = 0;
	return TRUE;
}
