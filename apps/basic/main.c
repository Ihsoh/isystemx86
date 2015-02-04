#include "ilib.h"
#include "lexer.h"
#include "die.h"

char * code = 	"SUB Sum(BYVAL min AS INTEGER)";

void main(void)
{
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
