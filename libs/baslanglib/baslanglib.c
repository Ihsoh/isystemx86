#include "baslanglib.h"
#include "types.h"

#include "dslib/dslib.h"
#include "dslib/list.h"
#include "dslib/linked_list.h"
#include "dslib/hashtable.h"

BASLANGLMalloc	baslangl_malloc	= NULL;
BASLANGLCalloc	baslangl_calloc	= NULL;
BASLANGLFree	baslangl_free	= NULL;
BASLANGLDie		baslangl_die	= NULL;

BOOL
baslangl_init(IN BASLANGLEnvironmentPtr env)
{
	if(	env == NULL
		|| env->baslangl_malloc == NULL
		|| env->baslangl_calloc == NULL
		|| env->baslangl_free == NULL
		|| env->baslangl_die == NULL)
		return FALSE;
	baslangl_malloc = env->baslangl_malloc;
	baslangl_calloc = env->baslangl_calloc;
	baslangl_free = env->baslangl_free;
	baslangl_die = env->baslangl_die;

	DSLEnvironment dslenv;
	dslenv.dsl_malloc = baslangl_malloc;
	dslenv.dsl_calloc = baslangl_calloc;
	dslenv.dsl_free = baslangl_free;
	if(!dsl_init(&dslenv))
		return FALSE;
	
	return TRUE;
}

BOOL
baslangl_init_context(OUT BASLANGLContextPtr context)
{
	if(context == NULL)
		return FALSE;
	context->tokens = dsl_lst_new();
	if(context->tokens == NULL)
		return FALSE;
	context->ppointer = 0;
	context->global_var_list = dsl_hashtable_new();
	if(context->global_var_list == NULL)
	{
		dsl_lst_free(context->tokens);
		return FALSE;
	}
	context->local_var_list_stack = dsl_lnklst_new();
	if(context->local_var_list_stack == NULL)
	{
		dsl_lst_free(context->tokens);
		dsl_hashtable_free(context->global_var_list);
		return FALSE;
	}
	return TRUE;
}
