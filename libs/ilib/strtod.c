#include "string.h"
#include "stdlib.h"

double strtod(const char * nptr, char ** endptr)
{
	char buffer[1024];
	char * s;
	uint len = strlen(nptr);
	int i;
	for(i = 0; i < len; i++)
		if(nptr[i] == '+' || nptr[i] == '-' || (nptr[i] >= '0' && nptr[i] <= '9'))
			break;
	s = nptr + i;
	len = strlen(s);
	for(i = 1; i < len; i++)
		if(!(s[i] >= '0' && s[i] <= '9') && s[i] != '.')
			break;
	strncpy(buffer, s, i);
	buffer[i] = '\0';
	if(endptr != NULL)
		*endptr = s + i;
	return atof(buffer);
}
