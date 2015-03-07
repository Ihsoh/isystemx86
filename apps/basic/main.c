#include "ilib.h"
#include "lexer.h"
#include "die.h"

#include "dslib/dslib.h"
#include "dslib/list.h"
#include "dslib/value.h"

char * code = 	"SUB Sum(BYVAL min AS INTEGER)";

int main(int argc, char * argv[])
{
	float d0 = 100.0f, d1 = 200.123f;
	d0 = d0 + d1 * 2.31f;
	printf("%f\n", d0);
	return 0;

	DSLEnvironment env;
	env.dsl_malloc = malloc;
	env.dsl_calloc = calloc;
	env.dsl_free = free;
	dsl_init(&env);

	DSLListPtr list = dsl_lst_new();
	uint32 ui;
	for(ui = 1; ui <= 100; ui++)
		dsl_lst_add_value(list, dsl_val_uint32(ui));
	uint32 sum = 0;
	for(ui = 0; ui < 100; ui++)
		sum += DSL_UINT32VAL(dsl_lst_get(list, ui));
	printf("Sum = %d\n", sum);
	return 0;

	int i, i1;
	create_file("DA:/", "test.txt");
	for(i = 0; i < 1; i++)
	{
		FILE * fptr = fopen("DA:/test.txt", FILE_MODE_WRITE | FILE_MODE_APPEND);
		if(fptr == NULL)
		{
			printf("ERROR!\n");
			return -1;
		}
		printf("==========%d==========\n", i);
		fwrite(fptr, "", 0);
		for(i1 = 0; i1 < 20; i1++)
		{
			char buffer[100];
			sprintf(buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ%d\n", i1);
			fappend(fptr, buffer, strlen(buffer));
		}
		fclose(fptr);
	}
	return 0;
	/*
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
	return 0;*/
}
