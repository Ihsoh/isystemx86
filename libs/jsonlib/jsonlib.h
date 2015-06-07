#ifndef	_JSONLIB_H_
#define	_JSONLIB_H_

#include "types.h"

#include <dslib/hashtable.h>
#include <dslib/list.h>

#define	JSONL_TYPE_OBJECT		1
#define	JSONL_TYPE_ARRAY		2
#define	JSONL_TYPE_VALUE		3
#define	JSONL_TYPE_NULL			4
#define	JSONL_TYPE_TRUE			5
#define	JSONL_TYPE_FALSE		6
#define	JSONL_TYPE_NUMBER		7

#define	JSONL_VALUE_TYPE_ERROR	-1
#define	JSONL_VALUE_TYPE_BOOL	1
#define	JSONL_VALUE_TYPE_NULL	2
#define	JSONL_VALUE_TYPE_STRING	3
#define	JSONL_VALUE_TYPE_NUMBER	4

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
	ASCCHAR		value[JSONL_MAX_VALUE_LEN + 1];
} JSONLValue, * JSONLValuePtr;

typedef struct _JSONLNull
{
	int32		type;
} JSONLNull, * JSONLNullPtr;

typedef struct _JSONLTrue
{
	int32		type;
} JSONLTrue, * JSONLTruePtr;

typedef struct _JSONLFalse
{
	int32		type;
} JSONLFalse, * JSONLFalsePtr;

typedef struct _JSONLNumber
{
	int32		type;
	double		number;
} JSONLNumber, * JSONLNumberPtr;

typedef struct _JSONLRaw
{
	int32		type;
} JSONLRaw, * JSONLRawPtr;

#define	JSONL_TYPE(ptr) (((JSONLRawPtr)ptr)->type)

#define	JSONL_VALUE(ptr)	(((JSONLValuePtr)(ptr))->value)
#define	JSONL_ARRAY(ptr)	((JSONLArrayPtr)(ptr))
#define	JSONL_OBJECT(ptr)	((JSONLObjectPtr)(ptr))

#define	JSONL_ARRAY_COUNT(ptr)		(((JSONLArrayPtr)(ptr))->array->count)
#define	JSONL_ARRAY_GET(ptr, idx)	((JSONLRawPtr)(dsl_lst_get(((JSONLArrayPtr)(ptr))->array, idx)->value.object_value))

#define	JSONL_OBJECT_GET(ptr, name, rawptr)	(_jsonl_object_get((ptr), (name), (rawptr)))

extern
BOOL
jsonl_init(IN JSONLEnvironmentPtr env);

extern
BOOL
_jsonl_object_get(	IN JSONLObjectPtr obj,
					IN int8 * name,
					OUT JSONLRawPtr * rawptr);

extern
int32
jsonl_value_type(IN JSONLRawPtr raw);

extern
BOOL
jsonl_number_value(	IN JSONLRawPtr raw,
					OUT double * v);

extern
BOOL
jsonl_bool_value(	IN JSONLRawPtr raw,
					OUT BOOL * v);

extern
BOOL
jsonl_string_value(	IN JSONLRawPtr raw,
					OUT int8 * v,
					IN uint32 max);

extern
JSONLRawPtr
jsonl_parse(IN int8 * json_text,
			OUT int8 ** next);

extern
JSONLRawPtr
jsonl_parse_json(IN int8 * json_text);

extern
BOOL
jsonl_free_json(IN JSONLRawPtr raw);

#endif
