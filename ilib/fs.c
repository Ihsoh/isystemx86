//Filename:		fs.c
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	File System Functions

#include <fs.h>
#include <types.h>
#include <sparams.h>
#include <system.h>

FILE * fopen(char * path, int mode)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(mode);
	system_call(SCALL_FS, SCALL_FOPEN, &sparams);
	return (FILE *)UINT32_PTR_SPARAM(sparams.param0);
}

int fclose(FILE * fptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	system_call(SCALL_FS, SCALL_FCLOSE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int fwrite(FILE * fptr, uchar * buffer, uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(buffer);
	sparams.param2 = SPARAM(len);
	system_call(SCALL_FS, SCALL_FWRITE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

uint fread(FILE * fptr, uchar * buffer, uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(buffer);
	sparams.param2 = SPARAM(len);
	system_call(SCALL_FS, SCALL_FREAD, &sparams);
	return UINT32_SPARAM(sparams.param0);
}

int fappend(FILE * fptr, uchar * buffer, uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(buffer);
	sparams.param2 = SPARAM(len);
	system_call(SCALL_FS, SCALL_FAPPEND, &sparams);
	return INT32_SPARAM(sparams.param0);
}

void freset(FILE * fptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	system_call(SCALL_FS, SCALL_FRESET, &sparams);
}

int exists_df(char * path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_FS, SCALL_EXISTS_DF, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int create_dir(char * path, char * name)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(name);
	system_call(SCALL_FS, SCALL_CREATE_DIR, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int create_file(char * path, char * name)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(name);
	system_call(SCALL_FS, SCALL_CREATE_FILE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int del_dir(char * path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_FS, SCALL_DEL_DIR, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int del_file(char * path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_FS, SCALL_DEL_FILE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int del_dirs(char * path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_FS, SCALL_DEL_DIRS, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int rename_dir(char * path, char * new_name)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(new_name);
	system_call(SCALL_FS, SCALL_RENAME_DIR, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int rename_file(char * path, char * new_name)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(new_name);
	system_call(SCALL_FS, SCALL_RENAME_FILE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

uint flen(FILE * fptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	system_call(SCALL_FS, SCALL_FLEN, &sparams);
	return INT32_SPARAM(sparams.param0);
}

void fcreate_dt(FILE * fptr, struct CMOSDateTime * dt)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(dt);
	system_call(SCALL_FS, SCALL_FCREATE_DT, &sparams);
}

void fchange_dt(FILE * fptr, struct CMOSDateTime * dt)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(fptr);
	sparams.param1 = SPARAM(dt);
	system_call(SCALL_FS, SCALL_FCHANGE_DT, &sparams);
}

int df_count(char * path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_FS, SCALL_DF_COUNT, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int df(char * path, struct RawBlock * raw_blocks)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(raw_blocks);
	system_call(SCALL_FS, SCALL_DF, &sparams);
	return INT32_SPARAM(sparams.param0);
}
