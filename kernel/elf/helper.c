#include "helper.h"

#include "../types.h"
#include "../fs/ifs1/fs.h"

uint32
ElfGetFileSize(IN CASCTEXT path)
{
	FileObjectPtr foptr = Ifs1OpenFile((CASCTEXT)path, FILE_MODE_READ);
	if(foptr == NULL)
		return 0;
	uint32 length = flen(foptr);
	Ifs1CloseFile(foptr);
	return length;
}
