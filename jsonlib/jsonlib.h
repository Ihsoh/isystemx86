#ifndef	_JSONLIB_H_
#define	_JSONLIB_H_

#include "types.h"

typedef void * (* JSONLMalloc)(IN uint32 num_bytes);
typedef void * (* JSONLCalloc)(IN uint32 n, IN uint32 size);
typedef void (* JSONLFree)(IN void * ptr);

typedef struct _JSONLEnvironment
{
	JSONLMalloc	jsonl_malloc;
	JSONLCalloc	jsonl_calloc;
	JSONLFree	jsonl_free;
} JSONLEnvironment;

extern JSONLMalloc	jsonl_malloc;
extern JSONLCalloc	jsonl_calloc;
extern JSONLFree	jsonl_free;

extern
BOOL
jsonl_init(IN JSONLEnvironment * env);

#endif
