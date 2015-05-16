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

#define	TOKEN 			(token(context))
#define	NEXT_TOKEN()	{ next_token(context); }

static
BOOL
atom(	IN OUT BASLANGLContextPtr context,
		OUT BASLANGLExprResultPtr result)
{
	if(context == NULL || result == NULL)
		return FALSE;
	if(TOKEN != NULL)
	{
		if(TOKEN[0] >= '0' && TOKEN[0] <= '9')
		{
			result->type = BASLANGL_EXPR_TYPE_DOUBLE;
			result->value.d = stod(TOKEN);
			NEXT_TOKEN();
		}
		else if(TOKEN[0] == '"')
		{

		}
		else
			baslangl_die("Invalid expression");
	}
	else
		baslangl_die("Expect expression");
	return TRUE;
}

