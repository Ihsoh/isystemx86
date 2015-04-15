#include "jsonlib.h"
#include "types.h"

#include <dslib/hashtable.h>
#include <dslib/list.h>
#include <dslib/value.h>

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
			{
				dsl_lst_delete_all_object_value(array->array);
				dsl_lst_free(array->array);
				jsonl_free(array);
				return NULL;
			}
			if(*json_text == ',')
				if(!has_value)
				{
					dsl_lst_delete_all_object_value(array->array);
					dsl_lst_free(array->array);
					jsonl_free(array);
					return NULL;
				}
				else
					json_text++;
			if(!IS_SPACE(*json_text))
			{
				has_value = TRUE;
				void * raw_value = NULL;
				int32 len = 0;
				if(*json_text == '"')
				{
					int8 * value = NULL;
					json_text++;
					value = (int8 *)jsonl_malloc(JSONL_MAX_VALUE_LEN + 1);
					if(value == NULL)
					{
						dsl_lst_delete_all_object_value(array->array);
						dsl_lst_free(array->array);
						jsonl_free(array);
						return NULL;
					}
					*(value + len++) = '"';
					while(*json_text != '"')
					{
						if(len >= JSONL_MAX_VALUE_LEN - 1 || *json_text == '\0')
						{
							jsonl_free(value);
							dsl_lst_delete_all_object_value(array->array);
							dsl_lst_free(array->array);
							jsonl_free(array);
							return NULL;
						}
						if(*json_text == '\\')
						{
							switch(*(++json_text))
							{
								case '\\':
									*(value + len++) = '\\';
									break;
								case 'r':
									*(value + len++) = '\r';
									break;
								case 'n':
									*(value + len++) = '\n';
									break;
								case 't':
									*(value + len++) = '\t';
									break;
								case 'v':
									*(value + len++) = '\v';
									break;
								default:
									jsonl_free(value);
									dsl_lst_delete_all_object_value(array->array);
									dsl_lst_free(array->array);
									jsonl_free(array);
									return NULL;
							}
							json_text++;
						}
						else
							*(value + len++) = *(json_text++);
					}
					*(value + len++) = '"';
					*(value + len) = '\0';
					json_text++;
					JSONLValuePtr jsonv = (JSONLValuePtr)jsonl_malloc(sizeof(JSONLValue));
					if(jsonv == NULL)
					{
						jsonl_free(value);
						dsl_lst_delete_all_object_value(array->array);
						dsl_lst_free(array->array);
						jsonl_free(array);
						return NULL;
					}
					jsonv->type = JSONL_TYPE_VALUE;
					jsonv->value.value.object_value = value;
					jsonv->value.type = DSLVALUE_OBJECT;
					raw_value = jsonv;
				}
				else if(*json_text == '[')
				{
					JSONLArrayPtr array_value = NULL;
					array_value = (JSONLArrayPtr)jsonl_parse(json_text, &json_text);
					if(array_value == NULL)
					{
						dsl_lst_delete_all_object_value(array->array);
						dsl_lst_free(array->array);
						jsonl_free(array);
						return NULL;
					}
					raw_value = array_value;
				}
				else if(*json_text == '{')
				{
					JSONLObjectPtr object_value = NULL;
					object_value = (JSONLObjectPtr)jsonl_parse(json_text, &json_text);
					if(object_value == NULL)
					{
						dsl_lst_delete_all_object_value(array->array);
						dsl_lst_free(array->array);
						jsonl_free(array);
						return NULL;
					}
					raw_value = object_value;
				}
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
			int8 key[DSLHASHTABLE_SIZE];
			int32 key_len = 0;
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
				if(*json_text == '"')
				{
					int8 * value = NULL;
					json_text++;
					value = (int8 *)jsonl_malloc(JSONL_MAX_VALUE_LEN + 1);
					if(value == NULL)
					{
						dsl_hashtable_unset_all(obj->obj);
						dsl_hashtable_free(obj->obj);
						jsonl_free(obj);
						return NULL;
					}
					*(value + len++) = '"';
					while(*json_text != '"')
					{
						if(len >= JSONL_MAX_VALUE_LEN - 1 || *json_text == '\0')
						{
							jsonl_free(value);
							dsl_hashtable_unset_all(obj->obj);
							dsl_hashtable_free(obj->obj);
							jsonl_free(obj);
							return NULL;
						}
						if(*json_text == '\\')
						{
							switch(*(++json_text))
							{
								case '\\':
									*(value + len++) = '\\';
									break;
								case 'r':
									*(value + len++) = '\r';
									break;
								case 'n':
									*(value + len++) = '\n';
									break;
								case 't':
									*(value + len++) = '\t';
									break;
								case 'v':
									*(value + len++) = '\v';
									break;
								default:
									jsonl_free(value);
									dsl_hashtable_unset_all(obj->obj);
									dsl_hashtable_free(obj->obj);
									jsonl_free(obj);
									return NULL;
							}
							json_text++;
						}
						else
							*(value + len++) = *(json_text++);
					}
					*(value + len++) = '"';
					*(value + len) = '\0';
					json_text++;
					JSONLValuePtr jsonv = (JSONLValuePtr)jsonl_malloc(sizeof(JSONLValue));
					if(jsonv == NULL)
					{
						jsonl_free(value);
						dsl_hashtable_unset_all(obj->obj);
						dsl_hashtable_free(obj->obj);
						jsonl_free(obj);
						return NULL;
					}
					jsonv->type = JSONL_TYPE_VALUE;
					jsonv->value.value.object_value = value;
					jsonv->value.type = DSLVALUE_OBJECT;
					raw_value = jsonv;
				}
				else if(*json_text == '[')
				{
					JSONLArrayPtr array_value = NULL;
					array_value = (JSONLArrayPtr)jsonl_parse(json_text, &json_text);
					if(array_value == NULL)
					{
						dsl_hashtable_unset_all(obj->obj);
						dsl_hashtable_free(obj->obj);
						jsonl_free(obj);
						return NULL;
					}
					raw_value = array_value;
				}
				else if(*json_text == '{')
				{
					JSONLObjectPtr object_value = NULL;
					object_value = (JSONLObjectPtr)jsonl_parse(json_text, &json_text);
					if(object_value == NULL)
					{
						dsl_hashtable_unset_all(obj->obj);
						dsl_hashtable_free(obj->obj);
						jsonl_free(obj);
						return NULL;
					}
					raw_value = object_value;
				}
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
	}
	else
		return NULL;
}
