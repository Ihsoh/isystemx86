#include "error.h"

#include <ilib/ilib.h>

void ErrorWithLine(char * Message, uint Line)
{
	assert(Message != NULL);

	printf("IASM Error(%d): %s\n", Line, Message);
	ILExitApplication();
}

void Error(char * Message)
{
	assert(Message != NULL);

	printf("IASM Error: %s\n", Message);
	ILExitApplication();
}
