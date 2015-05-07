#include "baslanglib.h"
#include "types.h"

#include "dslib/dslib.h"

BASLANGLMalloc	baslangl_malloc	= NULL;
BASLANGLCalloc	baslangl_calloc	= NULL;
BASLANGLFree	baslangl_free	= NULL;

BOOL
baslangl_init(IN BASLANGLEnvironmentPtr env)
{
	if(	env == NULL
		|| env->baslangl_malloc == NULL
		|| env->baslangl_calloc == NULL
		|| env->baslangl_free == NULL)
		return FALSE;
	baslangl_malloc = env->baslangl_malloc;
	baslangl_calloc = env->baslangl_calloc;
	baslangl_free = env->baslangl_free;

	DSLEnvironment dslenv;
	dslenv.dsl_malloc = baslangl_malloc;
	dslenv.dsl_calloc = baslangl_calloc;
	dslenv.dsl_free = baslangl_free;
	if(!dsl_init(&dslenv))
		return FALSE;
	
	return TRUE;
}
