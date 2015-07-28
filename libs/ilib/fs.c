//Filename:		fs.c
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	File System Functions

#include "fs.h"
#include "types.h"
#include "sparams.h"
#include "system.h"

#define	MAX_RETRY_COUNT	0x100

static int _lock_fs(void)
{
	struct SParams sparams;
	system_call(SCALL_FS, SCALL_LOCK_FS, &sparams);
	return INT32_SPARAM(sparams.param0);
}

static void _unlock_fs(void)
{
	struct SParams sparams;
	system_call(SCALL_FS, SCALL_UNLOCK_FS, &sparams);
}

static int _fs_lock_state(void)
{
	struct SParams sparams;
	system_call(SCALL_FS, SCALL_FS_LOCK_STATE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

static ILFILE * _fopen(char * path, int mode)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(mode);
	system_call(SCALL_FS, SCALL_FOPEN, &sparams);
	return (ILFILE *)UINT32_PTR_SPARAM(sparams.param0);
}

ILFILE * ILOpenFile(char * path, int mode)
{
	while(!_lock_fs())
		asm volatile ("pause");
	ILFILE * fptr = NULL;
	uint ui;
	for(ui = 0; ui < MAX_RETRY_COUNT && fptr == NULL; ui++)
		fptr = _fopen(path, mode);
	_unlock_fs();
	return fptr;
}

static int _fclose(ILFILE * fptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	system_call(SCALL_FS, SCALL_FCLOSE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILCloseFile(ILFILE * fptr)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _fclose(fptr);
	_unlock_fs();
	return retvalue;
}

static int _fwrite(ILFILE * fptr, uchar * buffer, uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(buffer);
	sparams.param2 = SPARAM(len);
	system_call(SCALL_FS, SCALL_FWRITE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILWriteFile(ILFILE * fptr, uchar * buffer, uint len)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _fwrite(fptr, buffer, len);
	_unlock_fs();
	return retvalue;
}

static uint _fread(ILFILE * fptr, uchar * buffer, uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(buffer);
	sparams.param2 = SPARAM(len);
	system_call(SCALL_FS, SCALL_FREAD, &sparams);
	return UINT32_SPARAM(sparams.param0);
}

uint ILReadFile(ILFILE * fptr, uchar * buffer, uint len)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _fread(fptr, buffer, len);
	_unlock_fs();
	return retvalue;
}

static int _fappend(ILFILE * fptr, uchar * buffer, uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(buffer);
	sparams.param2 = SPARAM(len);
	system_call(SCALL_FS, SCALL_FAPPEND, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILAppendFile(ILFILE * fptr, uchar * buffer, uint len)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _fappend(fptr, buffer, len);
	_unlock_fs();
	return retvalue;
}

void ILResetFile(ILFILE * fptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	system_call(SCALL_FS, SCALL_FRESET, &sparams);
}

static int _exists_df(char * path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_FS, SCALL_EXISTS_DF, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILPathExists(char * path)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _exists_df(path);
	_unlock_fs();
	return retvalue;
}

static int _create_dir(char * path, char * name)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(name);
	system_call(SCALL_FS, SCALL_CREATE_DIR, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILCreateDirectory(char * path, char * name)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _create_dir(path, name);
	_unlock_fs();
	return retvalue;
}

static int _create_file(char * path, char * name)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(name);
	system_call(SCALL_FS, SCALL_CREATE_FILE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILCreateFile(char * path, char * name)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _create_file(path, name);
	_unlock_fs();
	return retvalue;
}

static int _del_dir(char * path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_FS, SCALL_DEL_DIR, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILDeleteDirectory(char * path)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _del_dir(path);
	_unlock_fs();
	return retvalue;
}

static int _del_file(char * path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_FS, SCALL_DEL_FILE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILDeleteFile(char * path)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _del_file(path);
	_unlock_fs();
	return retvalue;
}

static int _del_dirs(char * path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_FS, SCALL_DEL_DIRS, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILDeleteDirectories(char * path)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _del_dirs(path);
	_unlock_fs();
	return retvalue;
}

static int _rename_dir(char * path, char * new_name)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(new_name);
	system_call(SCALL_FS, SCALL_RENAME_DIR, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILRenameDirectory(char * path, char * new_name)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _rename_dir(path, new_name);
	_unlock_fs();
	return retvalue;
}

static int _rename_file(char * path, char * new_name)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(new_name);
	system_call(SCALL_FS, SCALL_RENAME_FILE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILRenameFile(char * path, char * new_name)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _rename_file(path, new_name);
	_unlock_fs();
	return retvalue;
}

uint ILGetFileLength(ILFILE * fptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	system_call(SCALL_FS, SCALL_FLEN, &sparams);
	return UINT32_SPARAM(sparams.param0);
}

static void _fcreate_dt(ILFILE * fptr, struct CMOSDateTime * dt)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(dt);
	system_call(SCALL_FS, SCALL_FCREATE_DT, &sparams);
}

void ILGetFileCreatedTime(ILFILE * fptr, struct CMOSDateTime * dt)
{
	while(!_lock_fs())
		asm volatile ("pause");
	_fcreate_dt(fptr, dt);
	_unlock_fs();
}

static void _fchange_dt(ILFILE * fptr, struct CMOSDateTime * dt)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(dt);
	system_call(SCALL_FS, SCALL_FCHANGE_DT, &sparams);
}

void ILGetFileChangedTime(ILFILE * fptr, struct CMOSDateTime * dt)
{
	while(!_lock_fs())
		asm volatile ("pause");
	_fchange_dt(fptr, dt);
	_unlock_fs();
}

static int _df_count(char * path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_FS, SCALL_DF_COUNT, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILGetDirectoryItemCount(char * path)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _df_count(path);
	_unlock_fs();
	return retvalue;
}

static int _df(char * path, struct RawBlock * raw_blocks, uint max)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(raw_blocks);
	sparams.param2 = SPARAM(max);
	system_call(SCALL_FS, SCALL_DF, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILGetDirectoryItems(char * path, struct RawBlock * raw_blocks, uint max)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _df(path, raw_blocks, max);
	_unlock_fs();
	return retvalue;
}

int ILFixPath(char * path, char * new_path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(new_path);
	system_call(SCALL_FS, SCALL_FIX_PATH, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int ILIsEndOfFile(ILFILE * fptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	system_call(SCALL_FS, SCALL_FEOF, &sparams);
	return INT32_SPARAM(sparams.param0);
}
