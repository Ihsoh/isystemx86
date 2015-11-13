#include <ilib/ilib.h>

typedef void (* EntryFunction)(void);

void main(void)
{
	char file[1024];
	get_param_w(NULL);
	get_param_w(file);
	get_param_w(file);
	if(strcmp(file, "") == 0)
	{
		print_err_str_p("ELF Loader: Expect ELF file!\n", CC_RED);
		app_exit();
	}
	EntryFunction entry = (EntryFunction)ILLoadELF(file);
	if(entry != NULL)
		entry();
	else
	{
		print_err_str_p("ELF Loader: Cannot load this ELF file or this is not a valid ELF file!\n", CC_RED);
		app_exit();
	}
	app_exit();
}
