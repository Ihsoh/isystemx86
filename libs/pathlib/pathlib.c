#include "pathlib.h"
#include "types.h"

PATHLMalloc	pathl_malloc	= NULL;
PATHLCalloc	pathl_calloc	= NULL;
PATHLFree	pathl_free		= NULL;

BOOL
pathl_init(IN PATHLEnvironmentPtr env)
{
	if(	env == NULL
		|| env->pathl_malloc == NULL
		|| env->pathl_calloc == NULL
		|| env->pathl_free == NULL)
		return FALSE;
	pathl_malloc = env->pathl_malloc;
	pathl_calloc = env->pathl_calloc;
	pathl_free = env->pathl_free;
	return TRUE;
}
