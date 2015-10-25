#include "jsonlib.h"
#include "types.h"

#include <dslib/dslib.h>
#include <dslib/hashtable.h>
#include <dslib/list.h>
#include <dslib/value.h>
#include <dslib/linked_list.h>

JSONLMalloc	jsonl_malloc	= NULL;
JSONLCalloc	jsonl_calloc	= NULL;
JSONLFree	jsonl_free		= NULL;

BOOL
jsonl_init(IN JSONLEnvironmentPtr env)
{
	if(	env == NULL
		|| env->jsonl_malloc == NULL
		|| env->jsonl_calloc == NULL
		|| env->jsonl_free == NULL)
		return FALSE;
	jsonl_malloc = env->jsonl_malloc;
	jsonl_calloc = env->jsonl_calloc;
	jsonl_free = env->jsonl_free;

	DSLEnvironment dslenv;
	dslenv.dsl_malloc = jsonl_malloc;
	dslenv.dsl_calloc = jsonl_calloc;
	dslenv.dsl_free = jsonl_free;
	if(!dsl_init(&dslenv))
		return FALSE;
	
	return TRUE;
}

#define	IS_SPACE(chr)	\
	((chr) == ' ' || (chr) == '\r' || (chr) == '\n' || (chr) == '\t')

#define	IS_KEY_CHAR(chr)	\
	(((chr) >= 'A' && (chr) <= 'Z') || ((chr) >= 'a' && (chr) <= 'z')|| ((chr) >= '0' && (chr) <= '9') || (chr) == '_')

BOOL
_jsonl_object_get(	IN JSONLObjectPtr obj,
					IN int8 * name,
					OUT JSONLRawPtr * rawptr)
{
	if(obj == NULL || name == NULL || rawptr == NULL)
		return FALSE;
	DSLValuePtr vptr = NULL;
	if(!dsl_hashtable_get_object(obj->obj, name, (object)&vptr))
		return FALSE;
	if(vptr->type != DSLVALUE_OBJECT)
		return FALSE;
	*rawptr = vptr->value.object_value;
	return TRUE;
}

int32
jsonl_value_type(IN JSONLRawPtr raw)
{
	if(raw == NULL)
		return JSONL_VALUE_TYPE_ERROR;
	if(	JSONL_TYPE(raw) == JSONL_TYPE_TRUE
		|| JSONL_TYPE(raw) == JSONL_TYPE_FALSE)
		return JSONL_VALUE_TYPE_BOOL;
	else if(JSONL_TYPE(raw) == JSONL_TYPE_NULL)
		return JSONL_VALUE_TYPE_NULL;
	else if(JSONL_TYPE(raw) == JSONL_TYPE_VALUE)
		return JSONL_VALUE_TYPE_STRING;
	else if(JSONL_TYPE(raw) == JSONL_TYPE_NUMBER)
		return JSONL_VALUE_TYPE_NUMBER;
	else if(JSONL_TYPE(raw) == JSONL_TYPE_UINT)
		return JSONL_VALUE_TYPE_UINT;
	else
		return JSONL_VALUE_TYPE_ERROR;
}

BOOL
jsonl_number_value(	IN JSONLRawPtr raw,
					OUT double * v)
{
	if(	raw == NULL 
		|| v == NULL 
		|| jsonl_value_type(raw) != JSONL_VALUE_TYPE_NUMBER)
		return FALSE;
	*v = ((JSONLNumberPtr)raw)->number;
	return TRUE;
}

BOOL
jsonl_bool_value(	IN JSONLRawPtr raw,
					OUT BOOL * v)
{
	if(	raw == NULL 
		|| v == NULL 
		|| jsonl_value_type(raw) != JSONL_VALUE_TYPE_BOOL)
		return FALSE;
	if(JSONL_TYPE(raw) == JSONL_TYPE_TRUE)
		*v = TRUE;
	else if(JSONL_TYPE(raw) == JSONL_TYPE_FALSE)
		*v = FALSE;
	else
		return FALSE;
	return TRUE;
}

BOOL
jsonl_string_value(	IN JSONLRawPtr raw,
					OUT int8 * v,
					IN uint32 max)
{
	if(	raw == NULL 
		|| v == NULL
		|| max == 0
		|| jsonl_value_type(raw) != JSONL_VALUE_TYPE_STRING
		|| JSONL_VALUE(raw)[0] != '"')
		return FALSE;
	max--;
	uint32 index = 0;
	uint32 len = jsonl_lib_strlen(JSONL_VALUE(raw)) - 2;
	while(max-- != 0 && len-- != 0)
		*(v++) = (JSONL_VALUE(raw) + 1)[index++];
	*v = '\0';
	return TRUE;
}

BOOL
jsonl_uint_value(	IN JSONLRawPtr raw,
					OUT uint32 * v)
{
	if(	raw == NULL
		|| v == NULL
		|| jsonl_value_type(raw) != JSONL_VALUE_TYPE_UINT)
		return FALSE;
	*v = ((JSONLUIntPtr)raw)->number;
	return TRUE;
}

JSONLRawPtr
jsonl_parse(IN int8 * json_text,
			OUT int8 ** next)
{
	if(json_text == NULL)
		return NULL;
	while(IS_SPACE(*json_text))
		json_text++;
	if(*json_text == '\0')
		return NULL;
	if(*json_text == '[')
	{
		json_text++;
		JSONLArrayPtr array = (JSONLArrayPtr)jsonl_malloc(sizeof(JSONLArray));
		if(array == NULL)
			return NULL;
		array->type = JSONL_TYPE_ARRAY;
		array->array = dsl_lst_new();
		if(array->array == NULL)
		{
			jsonl_free(array);
			return NULL;
		}
		BOOL has_value = FALSE;
		while(*json_text != ']')
		{
			if(*json_text == '\0')
				goto array_error;
			if(*json_text == ',')
				if(!has_value)
					goto array_error;
				else
					json_text++;
			if(!IS_SPACE(*json_text))
			{
				has_value = TRUE;
				void * raw_value = NULL;
				int32 len = 0;

				#define	__ERROR_LABEL__ array_error
				#include "jsonlib.1.c"
				#undef	__ERROR_LABEL__

				DSLValuePtr v = (DSLValuePtr)dsl_val_object(raw_value);
				if(v == NULL)
				{
					jsonl_free(raw_value);
					dsl_lst_delete_all_object_value(array->array);
					dsl_lst_free(array->array);
					jsonl_free(array);
					return NULL;
				}
				if(!dsl_lst_add_value(array->array, v))
				{
					jsonl_free(v);
					jsonl_free(raw_value);
					dsl_lst_delete_all_object_value(array->array);
					dsl_lst_free(array->array);
					jsonl_free(array);
					return NULL;
				}
			}
			else
				json_text++;
		}
		json_text++;
		if(next != NULL)
			*next = json_text;
		return array;
array_error:
		dsl_lst_delete_all_object_value(array->array);
		dsl_lst_free(array->array);
		jsonl_free(array);
		return NULL;
	}
	else if(*json_text == '{')
	{
		json_text++;
		JSONLObjectPtr obj = (JSONLObjectPtr)jsonl_malloc(sizeof(JSONLObject));
		if(obj == NULL)
			return NULL;
		obj->type = JSONL_TYPE_OBJECT;
		obj->obj = dsl_hashtable_new();
		if(obj->obj == NULL)
		{
			jsonl_free(obj);
			return NULL;
		}
		BOOL has_value = FALSE;
		BOOL state_value = FALSE;
		int8 key[DSLHASHTABLE_SIZE];
		int32 key_len = 0;
		while(*json_text != '}')
		{
			if(*json_text == '\0')
			{
				dsl_hashtable_unset_all(obj->obj);
				dsl_hashtable_free(obj->obj);
				jsonl_free(obj);
				return NULL;
			}
			if(*json_text == ',')
				if(!has_value)
				{
					dsl_hashtable_unset_all(obj->obj);
					dsl_hashtable_free(obj->obj);
					jsonl_free(obj);
					return NULL;
				}
				else
					json_text++;
			key[0] = '\0';
			key_len = 0;
			if(!state_value && !IS_SPACE(*json_text))
			{
				while(!IS_SPACE(*json_text) && *json_text != ':')
				{
					if(	key_len >= DSLHASHTABLE_MAX_NAME_LEN
						|| *json_text == '\0'
						|| !IS_KEY_CHAR(*json_text))
					{
						dsl_hashtable_unset_all(obj->obj);
						dsl_hashtable_free(obj->obj);
						jsonl_free(obj);
						return NULL;
					}
					else
						key[key_len++] = *(json_text++);
				}
				key[key_len] = '\0';
				while(IS_SPACE(*json_text))
					json_text++;
				if(*json_text != ':')
				{
					dsl_hashtable_unset_all(obj->obj);
					dsl_hashtable_free(obj->obj);
					jsonl_free(obj);
					return NULL;
				}
				json_text++;
				state_value = TRUE;
			}
			if(state_value && !IS_SPACE(*json_text))
			{
				has_value = TRUE;
				void * raw_value = NULL;
				int32 len = 0;
				
				#define	__ERROR_LABEL__ obj_error
				#include "jsonlib.1.c"
				#undef	__ERROR_LABEL__

				DSLValuePtr v = (DSLValuePtr)dsl_val_object(raw_value);
				if(v == NULL)
				{
					jsonl_free(raw_value);
					dsl_hashtable_unset_all(obj->obj);
					dsl_hashtable_free(obj->obj);
					jsonl_free(obj);
					return NULL;
				}
				if(!dsl_hashtable_set_object(obj->obj, key, v))
				{
					jsonl_free(v);
					jsonl_free(raw_value);
					dsl_hashtable_unset_all(obj->obj);
					dsl_hashtable_free(obj->obj);
					jsonl_free(obj);
					return NULL;
				}
				state_value = FALSE;
			}
			else
				json_text++;
		}
		json_text++;
		if(next != NULL)
			*next = json_text;
		return obj;
obj_error:
		dsl_hashtable_unset_all(obj->obj);
		dsl_hashtable_free(obj->obj);
		jsonl_free(obj);
		return NULL;
	}
	else
		return NULL;
}

JSONLRawPtr
jsonl_parse_json(IN int8 * json_text)
{
	return jsonl_parse(json_text, NULL);
}

BOOL
jsonl_free_json(IN JSONLRawPtr raw)
{
	if(raw == NULL)
		return FALSE;
	switch(raw->type)
	{
		case JSONL_TYPE_VALUE:
		case JSONL_TYPE_NULL:
		case JSONL_TYPE_TRUE:
		case JSONL_TYPE_FALSE:
		case JSONL_TYPE_NUMBER:
		case JSONL_TYPE_UINT:
			jsonl_free(raw);
			break;
		case JSONL_TYPE_ARRAY:
		{
			JSONLArrayPtr array = (JSONLArrayPtr)raw;
			int32 i;
			int32 count = array->array->count;
			for(i = 0; i < count; i++)
			{
				DSLValuePtr vptr = dsl_lst_get(array->array, i);
				if(vptr == NULL)
					return FALSE;
				JSONLRawPtr rawvptr = (JSONLRawPtr)(vptr->value.object_value);
				if(rawvptr == NULL)
					return FALSE;
				if(!jsonl_free_json(rawvptr))
					return FALSE;
				jsonl_free(vptr);
			}
			if(!dsl_lst_free(array->array))
				return FALSE;
			jsonl_free(array);
			break;
		}
		case JSONL_TYPE_OBJECT:
		{
			JSONLObjectPtr obj = (JSONLObjectPtr)raw;
			DSLLinkedListPtr keys_list = dsl_lnklst_new();
			if(keys_list == NULL)
				return FALSE;
			if(!dsl_hashtable_keys_list(obj->obj, keys_list))
				return FALSE;
			DSLLinkedListNodePtr node = keys_list->head;
			while(node != NULL)
			{
				CASCTEXT key = (CASCTEXT)(node->value.value.object_value);
				DSLValuePtr vptr = NULL;
				if(!dsl_hashtable_get_object(obj->obj, key, (object *)&vptr))
					return FALSE;
				JSONLRawPtr rawvptr = (JSONLRawPtr)(vptr->value.object_value);
				if(!jsonl_free_json(rawvptr))
					return FALSE;
				jsonl_free(vptr);
				node = node->next;
			}
			if(	!dsl_hashtable_free_keys_list_items(keys_list)
				|| !dsl_lnklst_free(keys_list)
				|| !dsl_hashtable_free(obj->obj))
				return FALSE;
			jsonl_free(obj);
			break;
		}
		default:
			return FALSE;
	}
	return TRUE;
}
