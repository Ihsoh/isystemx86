#include "die.h"
#include "ilib.h"

void die(const char * message)
{
	printf("\nERROR: %s\n", message);
	app_exit();
}