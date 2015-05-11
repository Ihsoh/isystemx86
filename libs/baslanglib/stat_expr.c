#include "stat_expr.h"
#include "types.h"
#include "baslanglib.h"

static
ASCTEXT
token(IN BASLANGLContextPtr context)
{
	if(	context == NULL
		|| context->ppointer >= context->tokens->count)
		return NULL;
	return dsl_lst_get(context->tokens, context->ppointer)->value.object_value;
}

static
BOOL
next_token(IN BASLANGLContextPtr context)
{
	if(	context == NULL
		|| context->ppointer + 1 >= context->tokens->count)
		return FALSE;
	context->ppointer++;
	return TRUE;
}

static
BOOL
atom(	IN OUT BASLANGLContextPtr context,
		OUT BASLANGLExprResultPtr result)
{
	if(context == NULL || result == NULL)
		return FALSE;
	
	return TRUE;
}

