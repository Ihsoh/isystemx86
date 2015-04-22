#include <ilib.h>
#include "ins.h"

void f(void)
{
	f();
}

int main(int argc, char * argv[])
{
	f();
	return 0;
}
