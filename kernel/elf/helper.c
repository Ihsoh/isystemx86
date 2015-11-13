#include "helper.h"

#include "../types.h"
#include "../fs/ifs1/fs.h"

unsigned int elf_get_file_size(const char * path)
{
	FileObjectPtr foptr = open_file((char *)path, FILE_MODE_READ);
	if(foptr == NULL)
		return 0;
	unsigned int length = flen(foptr);
	close_file(foptr);
	return length;
}
