#include <ilib.h>
#include "ins.h"

int main(int argc, char * argv[])
{
	print_char_p('A', CC_GREEN);
	print_char('B');
	print_str_p("C", CC_GREEN);
	print_str("D");
	print_err_char_p('E', CC_RED);
	print_err_char('F');
	print_err_str_p("G", CC_RED);
	print_err_str("H");
	return 0;
}
