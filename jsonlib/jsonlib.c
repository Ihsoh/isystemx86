#include "jsonlib.h"
#include "types.h"

JSONLMalloc	jsonl_malloc	= NULL;
JSONLCalloc	jsonl_calloc	= NULL;
JSONLFree	jsonl_free		= NULL;

BOOL
jsonl_init(IN JSONLEnvironment * env)
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
