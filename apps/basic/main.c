#include <ilib/ilib.h>
#include <baslanglib/baslanglib.h>
#include "die.h"
int main(int argc, char * argv[])
{
	freopen("DA:/out.txt", "w", stdout);

    char *endptr;
    char a[] = "12345.6789";
    char b[] = "1234.567qwer";
    char c[] = "-232.23e4";
    printf( "a=%lf\n", strtod(a,NULL) );
    printf( "b=%lf\n", strtod(b,&endptr) );
    printf( "endptr=%s\n", endptr );
    printf( "c=%lf\n", strtod(c,NULL) );
    printf("%f\n", atof(a));

    fclose(stdout);
	printf("%s\n", "OK!");

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
