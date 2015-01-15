#include "ilib.h"
#include "lexer.h"

char * code = 	"SUB Sum100";

void main(void)
{
	printf_my("1\n2\n3\n");
	printf("1\n2\n3\n");
	app_exit();

	struct LEXWord * words = calloc(500, sizeof(struct LEXWord));
	if(words == NULL)
	{
		printf("Cannot alloc memory!\n"); 
		app_exit();
	}
	lexer(code, "main.bas", 500, words);
	int i;
	for(i = 0; i < 500; i++, words++)
		if(words->type == LEXWORD_EOF)
			break;
		else
			printf("%s\n", words->name);
	free(words);
	app_exit();
}

void printf_my(char * str)
{
	printf(str);
}
