#include "ilib.h"

#define	MAX_ARG_COUNT	256
#define MAX_ARG_LENGTH	1024

extern int main(int argc, char * argv[]);

void mta32main(void)
{
	int argc = 0;
	char * argv[MAX_ARG_COUNT];
	get_param_w(NULL);
	int i;
	for(i = 0; i < MAX_ARG_COUNT; i++)
	{
		argv[i] = allocm(MAX_ARG_LENGTH);
		if(argv[i] == NULL)
		{
			print_err_str_p("MTA32Main: Cannot allocate memory!\n", CC_RED);
			app_exit();
		}
		get_param_w(argv[i]);
		if(strcmp(argv[i], "") == 0)
			break;
		argc++;
	}
	int retvalue = main(argc, argv);
	for(i = 0; i < argc; i++)
		freem(argv[i]);
	__set_retvalue(retvalue);
	app_exit();
}
