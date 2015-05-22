#include <ilib/ilib.h>
#include <baslanglib/baslanglib.h>
#include "die.h"

int comparetor(__const __ptr_t a, __const __ptr_t b)
{
	if(*(uint32 *)a > *(uint32 *)b)
		return 1;
	else if(*(uint32 *)a < *(uint32 *)b)
		return -1;
	else
		return 0;
}

int main(int argc, char * argv[])
{
	uint32 arr[] = {100, 2, 3132, 23, 1, 32, 111, 33, 232131, 32};
	qsort(arr, sizeof(arr) / sizeof(uint32), sizeof(uint32), comparetor);
	uint32 ui;
	for(ui = 0; ui < sizeof(arr) / sizeof(uint32); ui++)
		printf("%u ", arr[ui]);
	print_str("\n");
	return 0;
	
	/*
	BASLANGLEnvironment env;
	env.baslangl_malloc = malloc;
	env.baslangl_calloc = calloc;
	env.baslangl_free = free;
	env.baslangl_die = die;
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
	lexer_free_tokens(tokens);
	*/
	return 0;
}
