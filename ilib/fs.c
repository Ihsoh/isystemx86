//Filename:		fs.c
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	File System Functions

#include <fs.h>
#include <types.h>
#include <sparams.h>
#include <system.h>

#define	MAX_RETRY_COUNT	0x10000

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

static FILE * _fopen(char * path, int mode)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(mode);
	system_call(SCALL_FS, SCALL_FOPEN, &sparams);
	return (FILE *)UINT32_PTR_SPARAM(sparams.param0);
}

FILE * fopen(char * path, int mode)
{
	while(!_lock_fs())
		asm volatile ("pause");
	FILE * fptr = NULL;
	fptr = _fopen(path, mode);
	_unlock_fs();
	return fptr;
}

static int _fclose(FILE * fptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	system_call(SCALL_FS, SCALL_FCLOSE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int fclose(FILE * fptr)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _fclose(fptr);
	_unlock_fs();
	return retvalue;
}

static int _fwrite(FILE * fptr, uchar * buffer, uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(buffer);
	sparams.param2 = SPARAM(len);
	system_call(SCALL_FS, SCALL_FWRITE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int fwrite(FILE * fptr, uchar * buffer, uint len)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _fwrite(fptr, buffer, len);
	_unlock_fs();
	return retvalue;
}

static uint _fread(FILE * fptr, uchar * buffer, uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(buffer);
	sparams.param2 = SPARAM(len);
	system_call(SCALL_FS, SCALL_FREAD, &sparams);
	return UINT32_SPARAM(sparams.param0);
}

uint fread(FILE * fptr, uchar * buffer, uint len)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _fread(fptr, buffer, len);
	_unlock_fs();
	return retvalue;
}

static int _fappend(FILE * fptr, uchar * buffer, uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(buffer);
	sparams.param2 = SPARAM(len);
	system_call(SCALL_FS, SCALL_FAPPEND, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int fappend(FILE * fptr, uchar * buffer, uint len)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _fappend(fptr, buffer, len);
	_unlock_fs();
	return retvalue;
}

void freset(FILE * fptr)
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

int exists_df(char * path)
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

int create_dir(char * path, char * name)
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

int create_file(char * path, char * name)
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

int del_dir(char * path)
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

int del_file(char * path)
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

int del_dirs(char * path)
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

int rename_dir(char * path, char * new_name)
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

int rename_file(char * path, char * new_name)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _rename_file(path, new_name);
	_unlock_fs();
	return retvalue;
}

uint flen(FILE * fptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	system_call(SCALL_FS, SCALL_FLEN, &sparams);
	return INT32_SPARAM(sparams.param0);
}

static void _fcreate_dt(FILE * fptr, struct CMOSDateTime * dt)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(dt);
	system_call(SCALL_FS, SCALL_FCREATE_DT, &sparams);
}

void fcreate_dt(FILE * fptr, struct CMOSDateTime * dt)
{
	while(!_lock_fs())
		asm volatile ("pause");
	_fcreate_dt(fptr, dt);
	_unlock_fs();
}

static void _fchange_dt(FILE * fptr, struct CMOSDateTime * dt)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(dt);
	system_call(SCALL_FS, SCALL_FCHANGE_DT, &sparams);
}

void fchange_dt(FILE * fptr, struct CMOSDateTime * dt)
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

int df_count(char * path)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _df_count(path);
	_unlock_fs();
	return retvalue;
}

static int _df(char * path, struct RawBlock * raw_blocks)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(raw_blocks);
	system_call(SCALL_FS, SCALL_DF, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int df(char * path, struct RawBlock * raw_blocks)
{
	while(!_lock_fs())
		asm volatile ("pause");
	int retvalue = 0;
	retvalue = _df(path, raw_blocks);
	_unlock_fs();
	return retvalue;
}

int fix_path(char * path, char * new_path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(new_path);
	system_call(SCALL_FS, SCALL_FIX_PATH, &sparams);
	return INT32_SPARAM(sparams.param0);
}
