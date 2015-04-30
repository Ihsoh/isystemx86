#include <ilib/ilib.h>
#include "ins.h"

int main(int argc, char * argv[])
{
	char buffer[6];
	get_strn(buffer, 5);
	print_str(buffer);
	return 0;
}
