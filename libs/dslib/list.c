#include "list.h"
#include "types.h"
#include "dslib.h"
#include "lib.h"

bool
dsl_lst_init_with_size(
	OUT DSLListPtr list,
	IN int32 size)
{
	if(list == NULL || size <= 0)
		return FALSE;
	list->values = (DSLValuePtr *)dsl_malloc(sizeof(DSLValuePtr) * size);
	if(list->values == NULL)
		return FALSE;
	list->size = size;
	list->count = 0;
	return TRUE;
}

bool
dsl_lst_init(OUT DSLListPtr list)
{
	return dsl_lst_init_with_size(list, DSL_LIST_DEFAULT_SIZE);
}

DSLListPtr
dsl_lst_new_with_size(IN int32 size)
{
	if(size <= 0)
		return NULL;
	DSLListPtr list = (DSLListPtr)dsl_malloc(sizeof(DSLList));
	if(list == NULL)
		return NULL;
	if(!dsl_lst_init_with_size(list, size))
	{
		dsl_free(list);
		return NULL;
	}
	return list;
}

DSLListPtr
dsl_lst_new(void)
{
	return dsl_lst_new_with_size(DSL_LIST_DEFAULT_SIZE);
}

BOOL
dsl_lst_release(IN DSLListPtr list)
{
	if(list == NULL)
		return FALSE;
	if(list->values != NULL)
		dsl_free(list->values);
	return TRUE;
}

BOOL
dsl_lst_free(IN DSLListPtr list)
{
	if(list == NULL)
		return FALSE;
	dsl_lst_release(list);
	dsl_free(list);
	return TRUE;
}

BOOL
dsl_lst_resize(IN OUT DSLListPtr list)
{
	if(list == NULL)
		return FALSE;
	int32 new_size = list->size * 2;
	DSLValuePtr * new_space = (DSLValuePtr *)dsl_malloc(sizeof(DSLValuePtr) * new_size);
	if(new_space == NULL)
		return FALSE;
	dsl_lib_memcpy(new_space, list->values, sizeof(DSLValuePtr) * list->size);
	dsl_free(list->values);
	list->values = new_space;
	list->size = new_size;
	return TRUE;
}

BOOL
dsl_lst_add_value(	IN OUT DSLListPtr list,
					IN DSLValuePtr value)
{
	if(list == NULL || value == NULL)
		return FALSE;
	if(list->count == list->size)
		if(!dsl_lst_resize(list))
			return FALSE;
	list->values[list->count++] = value;
	return TRUE;
}

BOOL
dsl_lst_remove_value_at_index(	IN OUT DSLListPtr list,
								IN int32 index)
{
	if(	list == NULL
		|| index < 0
		|| index >= list->count)
		return FALSE;
	uint32 ui;
	for(ui = index; ui < list->count - 1; ui++)
		list->values[ui] = list->values[ui + 1];
	list->count--;
	return TRUE;
}

int32
dsl_lst_find_value(	IN DSLListPtr list,
					IN DSLValuePtr value)
{
	if(list == NULL)
		return -1;
	int32 i;
	for(i = 0; i < list->count; i++)
		if(list->values[i] == value)
			return i;
	return -1;
}

BOOL
dsl_lst_remove_value(	IN OUT DSLListPtr list,
						IN DSLValuePtr value)
{
	return dsl_lst_remove_value_at_index(list, dsl_lst_find_value(list, value));
}

BOOL
dsl_lst_insert_value_at_index(	IN OUT DSLListPtr list,
								IN int32 index,
								IN DSLValuePtr value)
{
	if(	list == NULL
		|| value == NULL
		|| index < 0
		|| index > list->count)
		return FALSE;
	if(list->count == list->size)
		if(!dsl_lst_resize(list))
			return FALSE;
	uint32 ui;
	for(ui = list->count; ui > index; ui--)
		list->values[ui] = list->values[ui - 1];
	list->values[index] = value;
	list->count++;
	return TRUE;
}

DSLValuePtr
dsl_lst_get(IN DSLListPtr list,
			IN int32 index)
{
	if(	list == NULL
		|| index < 0
		|| index >= list->count)
		return NULL;
	return list->values[index];
}

BOOL
dsl_lst_set(IN DSLListPtr list,
			IN int32 index,
			IN DSLValuePtr value)
{
	if(	list == NULL
		|| index < 0
		|| index >= list->count)
		return FALSE;
	list->values[index] = value;
	return TRUE;
}

BOOL
dsl_lst_clear(IN DSLListPtr list)
{
	if(list == NULL)
		return FALSE;
	list->count = 0;
	return TRUE;
}

BOOL
dsl_lst_delete_value_at_index(	IN OUT DSLListPtr list,
								IN int32 index)
{
	DSLValuePtr value = dsl_lst_get(list, index);
	if(value == NULL)
		return FALSE;
	if(!dsl_lst_remove_value_at_index(list, index))
		return FALSE;
	dsl_free(value);
	return TRUE;
}

BOOL
dsl_lst_delete_object_value_at_index(	IN OUT DSLListPtr list,
										IN int32 index)
{
	DSLValuePtr value = dsl_lst_get(list, index);
	if(value == NULL)
		return FALSE;
	if(!dsl_lst_remove_value_at_index(list, index))
		return FALSE;
	dsl_free(value->value.object_value);
	dsl_free(value);
	return TRUE;
}

BOOL
dsl_lst_delete_all_value(IN OUT DSLListPtr list)
{
	if(list == NULL)
		return FALSE;
	while(list->count != 0)
		if(!dsl_lst_delete_value_at_index(list, list->count - 1))
			return FALSE;
	return TRUE;
}

BOOL
dsl_lst_delete_all_object_value(IN OUT DSLListPtr list)
{
	if(list == NULL)
		return NULL;
	while(list->count != 0)
		if(!dsl_lst_delete_object_value_at_index(list, list->count - 1))
			return FALSE;
	return TRUE;
}
