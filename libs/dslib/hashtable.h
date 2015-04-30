#ifndef	_HASHTABLE_H_
#define	_HASHTABLE_H_

#include "types.h"
#include "value.h"
#include "linked_list.h"

#define	DSLHASHTABLE_SIZE			256
#define	DSLHASHTABLE_MAX_NAME_LEN	255

typedef struct _DSLHashTable
{
	DSLLinkedListPtr table[DSLHASHTABLE_SIZE];
} DSLHashTable, * DSLHashTablePtr;

typedef struct _DSLHashTablePair
{
	uint8		name[DSLHASHTABLE_MAX_NAME_LEN + 1];
	DSLValue	value;
} DSLHashTablePair, * DSLHashTablePairPtr;

extern
DSLHashTablePtr
dsl_hashtable_new(void);

#define	EXTERN_DSL_HASHTABLE_SET_XXX(xxx)	\
	extern 	\
	BOOL	\
	dsl_hashtable_set_##xxx(	IN OUT DSLHashTablePtr hashtable,	\
								IN int8 * name,	\
								IN xxx value);

EXTERN_DSL_HASHTABLE_SET_XXX(int8)
EXTERN_DSL_HASHTABLE_SET_XXX(int16)
EXTERN_DSL_HASHTABLE_SET_XXX(int32)
EXTERN_DSL_HASHTABLE_SET_XXX(int64)
EXTERN_DSL_HASHTABLE_SET_XXX(uint8)
EXTERN_DSL_HASHTABLE_SET_XXX(uint16)
EXTERN_DSL_HASHTABLE_SET_XXX(uint32)
EXTERN_DSL_HASHTABLE_SET_XXX(uint64)
EXTERN_DSL_HASHTABLE_SET_XXX(float)
EXTERN_DSL_HASHTABLE_SET_XXX(double)
EXTERN_DSL_HASHTABLE_SET_XXX(object)
EXTERN_DSL_HASHTABLE_SET_XXX(bool)

#define	EXTERN_DSL_HASHTABLE_GET_XXX(xxx)	\
	extern 	\
	BOOL	\
	dsl_hashtable_get_##xxx(IN OUT DSLHashTablePtr hashtable,	\
							IN int8 * name,	\
							OUT xxx * value);

EXTERN_DSL_HASHTABLE_GET_XXX(int8)
EXTERN_DSL_HASHTABLE_GET_XXX(int16)
EXTERN_DSL_HASHTABLE_GET_XXX(int32)
EXTERN_DSL_HASHTABLE_GET_XXX(int64)
EXTERN_DSL_HASHTABLE_GET_XXX(uint8)
EXTERN_DSL_HASHTABLE_GET_XXX(uint16)
EXTERN_DSL_HASHTABLE_GET_XXX(uint32)
EXTERN_DSL_HASHTABLE_GET_XXX(uint64)
EXTERN_DSL_HASHTABLE_GET_XXX(float)
EXTERN_DSL_HASHTABLE_GET_XXX(double)
EXTERN_DSL_HASHTABLE_GET_XXX(object)
EXTERN_DSL_HASHTABLE_GET_XXX(bool)

extern
BOOL
dsl_hashtable_unset(IN OUT DSLHashTablePtr hashtable,
					IN const int8 * name);

extern
BOOL
dsl_hashtable_unset_all(IN OUT DSLHashTablePtr hashtable);

extern
BOOL
dsl_hashtable_free(IN OUT DSLHashTablePtr hashtable);

extern
BOOL
dsl_hashtable_has_key(	IN DSLHashTablePtr hashtable,
						IN const int8 * name);

extern
BOOL
dsl_hashtable_keys_list(IN DSLHashTablePtr hashtable,
						IN OUT DSLLinkedListPtr keys_list);

extern
BOOL
dsl_hashtable_free_keys_list_items(IN OUT DSLLinkedListPtr keys_list);

#endif
