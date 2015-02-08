#include "dslib.h"
#include "types.h"

DSLMalloc	dsl_malloc	= NULL;
DSLCalloc	dsl_calloc	= NULL;
DSLFree		dsl_free	= NULL;

BOOL
dsl_init(IN DSLEnvironment * env)
{
	if(	env == NULL
		|| env->dsl_malloc == NULL
		|| env->dsl_calloc == NULL
		|| env->dsl_free == NULL)
		return FALSE;
	dsl_malloc = env->dsl_malloc;
	dsl_calloc = env->dsl_calloc;
	dsl_free = env->dsl_free;
	return TRUE;
}
