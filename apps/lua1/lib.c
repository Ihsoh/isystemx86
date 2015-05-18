#include <ilib/ilib.h>

char * strdup(const char * s)
{
	uint len = strlen(s) + 1;
	char * new_str = (char *)malloc(len);
	if(new_str == NULL)
		return NULL;
	memcpy(new_str, s, len);
	return new_str;
}
