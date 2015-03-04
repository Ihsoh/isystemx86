#include "ilib.h"
#include "lexer.h"
#include "die.h"

#include "dslib/dslib.h"
#include "dslib/linked_list.h"

char * code = 	"SUB Sum(BYVAL min AS INTEGER)";

int main(int argc, char * argv[])
{
	
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
