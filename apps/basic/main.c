#include "ilib.h"
#include "lexer.h"
#include "die.h"

#include "dslib.h"
#include "linked_list.h"

char * code = 	"SUB Sum(BYVAL min AS INTEGER)";

void main(void)
{
	DSLEnvironment env;
	env.dsl_malloc = malloc;
	env.dsl_calloc = calloc;
	env.dsl_free = free;
	dsl_init(&env);
	if(dsl_lnklst_test())
		printf("OK!\n");
	else
		printf("Failed!\n");
	app_exit();

	struct LEXWord * words = calloc(500, sizeof(struct LEXWord));
	if(words == NULL)
	{
		printf("Cannot alloc memory!\n"); 
		app_exit();
	}
	lexer(code, "main.bas", 100, words);
	int i;
	while(words->type != LEXWORD_EOF)
		printf("%s\n", (words++)->name);
	free(words);
	app_exit();
}
