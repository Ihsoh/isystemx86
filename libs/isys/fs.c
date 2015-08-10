#include "fs.h"
#include "types.h"

#include "ilib/ilib.h"

ISYSFilePtr
ISYSOpenFile(	IN BYTEPTR path,
				IN UINT32 mode)
{
	if(path == NULL)
		return NULL;
	return il_fopen(path, mode);
}

BOOL
ISYSCloseFile(IN ISYSFilePtr file)
{
	if(file == NULL)
		return FALSE;
	return il_fclose(file);
}

BOOL
ISYSWriteFile(	IN ISYSFilePtr file,
				IN BYTEPTR data,
				IN UINT32 length)
{
	if(	file == NULL
		|| data == NULL)
		return FALSE;
	return il_fwrite(file, data, length);
}

UINT32
ISYSReadFile(	IN ISYSFilePtr file,
				OUT BYTEPTR data,
				IN UINT32 length)
{
	if(	file == NULL
		|| data == NULL)
		return 0;
	return il_fread(file, data, length);
}

BOOL
ISYSAppendFile(	IN ISYSFilePtr file,
				IN BYTEPTR data,
				IN UINT32 length)
{
	if(	file == NULL
		|| data == NULL)
		return FALSE;
	return il_fappend(file, data, length);
}

BOOL
ISYSResetFile(IN ISYSFilePtr file)
{
	if(file == NULL)
		return FALSE;
	il_freset(file);
	return TRUE;
}

BOOL
ISYSFEOF(IN ISYSFilePtr file)
{
	if(file == NULL)
		return FALSE;
	return il_feof(file);
}

UINT32
ISYSGetFileLength(IN ISYSFilePtr file)
{
	if(file == NULL)
		return 0;
	return il_flen(file);
}

BOOL
ISYSFileExists(IN BYTEPTR path)
{
	if(path == NULL)
		return FALSE;
	UINT32 length = strlen(path);
	if(length == 0 || path[length - 1] == '/')
		return FALSE;
	return il_exists_df(path);
}

BOOL
ISYSDirectoryExists(IN BYTEPTR path)
{
	if(path == NULL)
		return FALSE;
	UINT32 length = strlen(path);
	if(length == 0 || path[length - 1] != '/')
		return FALSE;
	return il_exists_df(path);
}

BOOL
ISYSExists(IN BYTEPTR path)
{
	if(path == NULL)
		return FALSE;
	return il_exists_df(path);
}

BOOL
ISYSCreateFile(IN BYTEPTR path)
{
	if(path == NULL)
		return FALSE;
	BYTE dirpath[1024];
	BYTE name[256];
	INT32 length = (INT32)strlen(path);
	if(length >= sizeof(dirpath))
		return FALSE;
	INT32 index = length - 1;
	for(; index >= 0; index--)
		if(path[index] == '/')
		{
			if(strlen(path + index) >= sizeof(name))
				return FALSE;
			strncpy(dirpath, path, index);
			dirpath[index] = '\0';
			strcpy(name, path + index);
			return il_create_file(dirpath, name);
		}
	return FALSE;
}

BOOL
ISYSCreateDirectory(IN BYTEPTR path)
{
	if(path == NULL)
		return FALSE;
	BYTE dirpath[1024];
	BYTE name[256];
	INT32 length = (INT32)strlen(path);
	if(length >= sizeof(dirpath))
		return FALSE;
	if(path[length - 1] != '/')
		return FALSE;
	INT32 index = length - 2;
	for(; index >= 0; index--)
		if(path[index] == '/')
		{
			UINT32 nmlength = (UINT32)length - ((UINT32)index + 1) - 1;
			if(strlen(path + index) >= sizeof(name))
				return FALSE;
			strncpy(dirpath, path, index + 1);
			dirpath[index + 1] = '\0';
			strncpy(name, path + index + 1, nmlength);
			name[nmlength] = '\0';
			return il_create_dir(dirpath, name);
		}
	return FALSE;
}
