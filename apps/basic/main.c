#include "ilib.h"
#include "lexer.h"
#include "die.h"

#include "dslib/dslib.h"
#include "dslib/list.h"
#include "dslib/value.h"

int main(int argc, char * argv[])
{
	DSLEnvironment env;
	env.dsl_malloc = malloc;
	env.dsl_calloc = calloc;
	env.dsl_free = free;
	dsl_init(&env);

	return 0;
}
