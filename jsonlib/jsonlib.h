#ifndef	_JSONLIB_H_
#define	_JSONLIB_H_

#include "types.h"

#include <dslib/hashtable.h>
#include <dslib/list.h>
#include <dslib/value.h>

#define	JSONL_TYPE_OBJECT		1
#define	JSONL_TYPE_ARRAY		2
#define	JSONL_TYPE_VALUE		3

#define	JSONL_MAX_VALUE_LEN		1023

typedef void * (* JSONLMalloc)(IN uint32 num_bytes);
typedef void * (* JSONLCalloc)(IN uint32 n, IN uint32 size);
typedef void (* JSONLFree)(IN void * ptr);

typedef struct _JSONLEnvironment
{
	JSONLMalloc	jsonl_malloc;
	JSONLCalloc	jsonl_calloc;
	JSONLFree	jsonl_free;
} JSONLEnvironment, * JSONLEnvironmentPtr;

extern JSONLMalloc	jsonl_malloc;
extern JSONLCalloc	jsonl_calloc;
extern JSONLFree	jsonl_free;

typedef struct _JSONLObject
{
	int32				type;
	DSLHashTablePtr		obj;
} JSONLObject, * JSONLObjectPtr;

typedef struct _JSONLArray
{
	int32		type;
	DSLListPtr	array;
} JSONLArray, * JSONLArrayPtr;

typedef struct _JSONLValue
{
	int32		type;
	DSLValue	value;
} JSONLValue, * JSONLValuePtr;

typedef struct _JSONLRaw
{
	int32		type;
} JSONLRaw, * JSONLRawPtr;

#define	JSONL_TYPE(ptr) (((JSONLRawPtr)ptr)->type)

#define	JSONL_VALUE(ptr)	((int8 *)(((JSONLValuePtr)ptr)->value.value.object_value))
#define	JSONL_ARRAY(ptr)	((JSONLArrayPtr)(ptr))
#define	JSONL_OBJECT(ptr)	((JSONLObjectPtr)(ptr))

#define	JSONL_ARRAY_COUNT(ptr)		(((JSONLArrayPtr)(ptr))->array->count)
#define	JSONL_ARRAY_GET(ptr, idx)	((JSONLRawPtr)(dsl_lst_get(((JSONLArrayPtr)(ptr))->array, idx)->value.object_value))

#define	JSONL_OBJECT_GET(ptr, name, rawptr)	(_jsonl_object_get((ptr), (name), (rawptr)))

extern
BOOL
jsonl_init(IN JSONLEnvironmentPtr env);

extern
_jsonl_object_get(	IN JSONLObjectPtr obj,
					IN int8 * name,
					OUT JSONLRawPtr * rawptr);

extern
JSONLRawPtr
jsonl_parse(IN int8 * json_text,
			OUT int8 ** next);

#endif
