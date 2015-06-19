#include "imagelib.h"
#include <dslib/dslib.h>
#include <ilib/ilib.h>

BOOL
imagelib_init(void)
{
	DSLEnvironment dslenv;
	dslenv.dsl_malloc = malloc;
	dslenv.dsl_calloc = calloc;
	dslenv.dsl_free = free;
	if(!dsl_init(&dslenv))
		return FALSE;
	return TRUE;
}
