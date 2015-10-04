#ifndef	_LINK_H_
#define	_LINK_H_

#include "types.h"
#include "value.h"

#define	DSL_LIST_DEFAULT_SIZE	16

typedef struct _DSLList
{
	DSLValuePtr *	values;
	int32			size;
	int32			count;
} DSLList, * DSLListPtr;

extern
DSLListPtr
dsl_lst_new_with_size(IN int32 size);

extern
DSLListPtr
dsl_lst_new(void);

extern
BOOL
dsl_lst_free(IN DSLListPtr list);

extern
BOOL
dsl_lst_resize(IN OUT DSLListPtr list);

extern
BOOL
dsl_lst_add_value(	IN OUT DSLListPtr list,
					IN DSLValuePtr value);

extern
BOOL
dsl_lst_remove_value_at_index(	IN OUT DSLListPtr list,
								IN int32 index);

extern
int32
dsl_lst_find_value(	IN DSLListPtr list,
					IN DSLValuePtr value);

extern
BOOL
dsl_lst_remove_value(	IN OUT DSLListPtr list,
						IN DSLValuePtr value);

extern
BOOL
dsl_lst_insert_value_at_index(	IN OUT DSLListPtr list,
								IN int32 index,
								IN DSLValuePtr value);

extern
DSLValuePtr
dsl_lst_get(IN DSLListPtr list,
			IN int32 index);

extern
BOOL
dsl_lst_set(IN DSLListPtr list,
			IN int32 index,
			IN DSLValuePtr value);

extern
BOOL
dsl_lst_clear(IN DSLListPtr list);

extern
BOOL
dsl_lst_delete_value_at_index(	IN OUT DSLListPtr list,
								IN int32 index);

extern
BOOL
dsl_lst_delete_object_value_at_index(	IN OUT DSLListPtr list,
										IN int32 index);

extern
BOOL
dsl_lst_delete_all_value(IN OUT DSLListPtr list);

extern
BOOL
dsl_lst_delete_all_object_value(IN OUT DSLListPtr list);

#endif
