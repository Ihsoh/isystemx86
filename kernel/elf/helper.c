#include "helper.h"

#include "../types.h"
#include "../fs/ifs1/fs.h"

unsigned int elf_get_file_size(const char * path)
{
	FileObjectPtr foptr = Ifs1OpenFile((char *)path, FILE_MODE_READ);
	if(foptr == NULL)
		return 0;
	unsigned int length = flen(foptr);
	Ifs1CloseFile(foptr);
	return length;
}
