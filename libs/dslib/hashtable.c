#include "hashtable.h"
#include "types.h"
#include "dslib.h"
#include "linked_list.h"

DSLHashTablePtr
dsl_hashtable_new(void)
{
	DSLHashTablePtr hashtable = dsl_malloc(sizeof(DSLHashTable));
	if(hashtable == NULL)
		return NULL;
	uint32 ui;
	for(ui = 0; ui < DSLHASHTABLE_SIZE; ui++)
		hashtable->table[ui] = NULL;
	return hashtable;
}

static
uint32
hash(IN const int8 * name)
{
	uint32 key = 0;
	uint32 len = dsl_lib_strlen(name);
	uint32 ui;
	for(ui = 0; ui < len; ui++)
		key += name[ui] * 31;
	return key % DSLHASHTABLE_SIZE;
}

static
DSLHashTablePairPtr
find_pair(	IN DSLHashTablePtr hashtable,
			IN const int8 * name)
{
	if(hashtable == NULL || name == NULL)
		return NULL;
	uint32 key = hash(name);
	DSLLinkedListPtr list = hashtable->table[key];
	if(list != NULL)
	{
		DSLLinkedListNodePtr node = list->head;
		while(node != NULL)
		{
			DSLHashTablePairPtr pair 
				= (DSLHashTablePairPtr)(node->value.value.object_value);
			if(node->value.type != DSLVALUE_OBJECT || pair == NULL)
				return NULL;
			if(dsl_lib_strcmp(pair->name, name) == 0)
				return pair;
			node = node->next;
		}
	}
	return NULL;
}

#define	DSL_HASHTABLE_SET_XXX(xxx, XXX)	\
	BOOL	\
	dsl_hashtable_set_##xxx(	IN OUT DSLHashTablePtr hashtable,	\
								IN int8 * name,	\
								IN xxx value)	\
	{	\
		if(hashtable == NULL || name == NULL)	\
			return FALSE;	\
		if(dsl_lib_strlen(name) > DSLHASHTABLE_MAX_NAME_LEN)	\
			return FALSE;	\
		uint32 key = hash(name);	\
		DSLLinkedListPtr list = hashtable->table[key];	\
		if(list == NULL)	\
		{	\
			list = dsl_lnklst_new();	\
			if(list == NULL)	\
				return FALSE;	\
			hashtable->table[key] = list;	\
		}	\
		if(find_pair(hashtable, name) != NULL)	\
			return FALSE;	\
		DSLHashTablePairPtr pair = dsl_malloc(sizeof(DSLHashTablePair));	\
		if(pair == NULL)	\
			return FALSE;	\
		dsl_lib_strcpy(pair->name, name);	\
		pair->value.type = DSLVALUE_##XXX;	\
		pair->value.value.xxx##_value = value;	\
		DSLLinkedListNodePtr node = dsl_lnklst_new_object_node(pair);	\
		if(node == NULL)	\
			return FALSE;	\
		if(!dsl_lnklst_add_node(list, node))	\
			return FALSE;	\
		return TRUE;	\
	}

DSL_HASHTABLE_SET_XXX(int8, INT8)
DSL_HASHTABLE_SET_XXX(int16, INT16)
DSL_HASHTABLE_SET_XXX(int32, INT32)
DSL_HASHTABLE_SET_XXX(int64, INT64)
DSL_HASHTABLE_SET_XXX(uint8, UINT8)
DSL_HASHTABLE_SET_XXX(uint16, UINT16)
DSL_HASHTABLE_SET_XXX(uint32, UINT32)
DSL_HASHTABLE_SET_XXX(uint64, UINT64)
DSL_HASHTABLE_SET_XXX(float, FLOAT)
DSL_HASHTABLE_SET_XXX(double, DOUBLE)
DSL_HASHTABLE_SET_XXX(object, OBJECT)
DSL_HASHTABLE_SET_XXX(bool, BOOL)

#define	DSL_HASHTABLE_GET_XXX(xxx, XXX)	\
	BOOL	\
	dsl_hashtable_get_##xxx(IN OUT DSLHashTablePtr hashtable,	\
							IN int8 * name,	\
							OUT xxx * value)	\
	{	\
		if(hashtable == NULL || name == NULL || value == NULL)	\
			return FALSE;	\
		DSLHashTablePairPtr pair = find_pair(hashtable, name);	\
		if(pair == NULL || pair->value.type != DSLVALUE_##XXX)	\
			return FALSE;	\
		*value = pair->value.value.xxx##_value;	\
		return TRUE;	\
	}

DSL_HASHTABLE_GET_XXX(int8, INT8)
DSL_HASHTABLE_GET_XXX(int16, INT16)
DSL_HASHTABLE_GET_XXX(int32, INT32)
DSL_HASHTABLE_GET_XXX(int64, INT64)
DSL_HASHTABLE_GET_XXX(uint8, UINT8)
DSL_HASHTABLE_GET_XXX(uint16, UINT16)
DSL_HASHTABLE_GET_XXX(uint32, UINT32)
DSL_HASHTABLE_GET_XXX(uint64, UINT64)
DSL_HASHTABLE_GET_XXX(float, FLOAT)
DSL_HASHTABLE_GET_XXX(double, DOUBLE)
DSL_HASHTABLE_GET_XXX(object, OBJECT)
DSL_HASHTABLE_GET_XXX(bool, BOOL)

BOOL
dsl_hashtable_unset(IN OUT DSLHashTablePtr hashtable,
					IN const int8 * name)
{
	if(hashtable == NULL || name == NULL)
		return FALSE;
	uint32 key = hash(name);
	DSLLinkedListPtr list = hashtable->table[key];
	if(list != NULL)
	{
		DSLLinkedListNodePtr node = list->head;
		while(node != NULL)
		{
			DSLHashTablePairPtr pair 
				= (DSLHashTablePairPtr)(node->value.value.object_value);
			if(node->value.type != DSLVALUE_OBJECT || pair == NULL)
				return NULL;
			if(dsl_lib_strcmp(pair->name, name) == 0)
				return dsl_lnklst_delete_object_node(list, node);
			node = node->next;
		}
	}
	return FALSE;
}

BOOL
dsl_hashtable_unset_all(IN OUT DSLHashTablePtr hashtable)
{
	if(hashtable == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < DSLHASHTABLE_SIZE; ui++)
		if(	hashtable->table[ui] != NULL
			&& !dsl_lnklst_delete_all_object_node(hashtable->table[ui]))
			return FALSE;
	return TRUE;
}

BOOL
dsl_hashtable_free(IN OUT DSLHashTablePtr hashtable)
{
	if(hashtable == NULL)
		return FALSE;
	if(!dsl_hashtable_unset_all(hashtable))
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < DSLHASHTABLE_SIZE; ui++)
		if(	hashtable->table[ui] != NULL
			&& !dsl_lnklst_free(hashtable->table[ui]))
				return FALSE;
	dsl_free(hashtable);
	return TRUE;
}

BOOL
dsl_hashtable_has_key(	IN DSLHashTablePtr hashtable,
						IN const int8 * name)
{
	return find_pair(hashtable, name) != NULL;
}

BOOL
dsl_hashtable_keys_list(IN DSLHashTablePtr hashtable,
						IN OUT DSLLinkedListPtr keys_list)
{
	if(hashtable == NULL || keys_list == NULL)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < DSLHASHTABLE_SIZE; ui++)
	{
		DSLLinkedListPtr list = hashtable->table[ui];
		if(list != NULL)
		{
			DSLLinkedListNodePtr node = list->head;
			while(node != NULL)
			{
				DSLHashTablePairPtr pair 
					= (DSLHashTablePairPtr)(node->value.value.object_value);
				if(node->value.type != DSLVALUE_OBJECT || pair == NULL)
					return FALSE;
				int8 * buffer = (int8 *)dsl_malloc(DSLHASHTABLE_MAX_NAME_LEN + 1);
				if(buffer == NULL)
				{
					dsl_lnklst_delete_all_object_node(keys_list);
					return FALSE;
				}
				dsl_lib_strcpy(buffer, pair->name);
				DSLLinkedListNodePtr key_node = dsl_lnklst_new_object_node(buffer);
				if(key_node == NULL)
				{
					dsl_free(buffer);
					dsl_lnklst_delete_all_object_node(keys_list);
					return FALSE;
				}
				dsl_lnklst_add_node(keys_list, key_node);
				node = node->next;
			}
		}
	}
	return TRUE;
}

BOOL
dsl_hashtable_free_keys_list_items(IN OUT DSLLinkedListPtr keys_list)
{
	if(keys_list == NULL)
		return FALSE;
	return dsl_lnklst_delete_all_object_node(keys_list);
}
