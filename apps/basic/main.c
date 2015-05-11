#include <ilib/ilib.h>
#include <baslanglib/baslanglib.h>
#include "die.h"

int main(int argc, char * argv[])
{
	/*
	BASLANGLEnvironment env;
	env.baslangl_malloc = malloc;
	env.baslangl_calloc = calloc;
	env.baslangl_free = free;
	if(!baslangl_init(&env))
		die("Cannot initialize 'baslanglib'.");

	
	#include <baslanglib/lexer.h>
	#include <dslib/list.h>
	DSLListPtr tokens = lexer("FUNCTION A(a, b)\n\tPRINT a + b + 1.0\nEND FUNCTION\n");
	if(tokens == NULL)
	{
		printf("ERROR!%d\n", lexer_error_no());
		return -1;
	}
	uint32 ui;
	for(ui = 0; ui < tokens->count; ui++)
		printf("%s|", dsl_lst_get(tokens, ui)->value.object_value);
	print_str("\n");
	lexer_free_tokens(tokens);*/

	#include <regexlib/regex.h>
	const char* rule = argv[1];
	const char* source = argv[2];

	printf("rule: %s\n", rule);
	printf("source: %s\n", source);

	struct reg_env* env = reg_open_env();

	struct reg_pattern* pattern = reg_new_pattern(env, rule);

	while(TRUE);
	
	//int success = reg_match(pattern, source, strlen(source));
	//printf("-------------- reslut -----------\n success: %d\n", success);
	reg_close_env(env);

	return 0;
}
