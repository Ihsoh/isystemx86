#include <ilib/ilib.h>
#include <baslanglib/baslanglib.h>
#include "die.h"

int main(int argc, char * argv[])
{
	char * str = " 123asd";
	char * s;
	printd(strtod(str, &s));
	print_str("\n");
	print_str(s);
	print_str("\n");
	return 0;

	FILE * fptr = fopen("DA:/isystem/123.txt", "wb+");
	fwrite("1234\n", 1, 5, fptr);
	fwrite("abcd\n", 1, 5, fptr);
	fputs("XXXX\n", fptr);
	fputs("\n", fptr);
	fputs("YYYY\n", fptr);
	fwrite("!@#$", 1, 4, fptr);
	fputc('A', fptr);
	fputc('\n', fptr);
	fprintf(fptr, "ABC: %d, DEF: %X\n", 123, 0x12345678);
	fclose(fptr);
	fptr = fopen("DA:/isystem/123.txt", "r+");
	if(fptr == NULL)
	{
		printf("Cannot open file to read!\n");
		return -1;
	}
	char buffer[100];
	ungetc('>', fptr);
	ungetc('<', fptr);
	while(fgets(buffer, 100, fptr) != NULL)
		printf("LINE: %s", buffer);
	fclose(fptr);
	fprintf(stdout, "|\n");

	return 0;

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
	
	return 0;
}
