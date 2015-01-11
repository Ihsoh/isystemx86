//Filename:		ifs1fs.h
//Author:		Ihsoh
//Date:			2014-7-18
//Descriptor:	File System

#ifndef	_IFS1FS_H_
#define	_IFS1FS_H_

#include "types.h"
#include "ifs1blocks.h"

#define	MAX_FILENAME_LEN		255
#define	MAX_FILENAME_BUFFER_LEN	(MAX_FILENAME_LEN + 1)
#define	MAX_DIRNAME_LEN			(MAX_FILENAME_LEN)
#define	MAX_DIRNAME_BUFFER_LEN	(MAX_DIRNAME_LEN + 1)
#define	MAX_PATH_LEN			1023
#define	MAX_PATH_BUFFER_LEN		(MAX_PATH_LEN + 1)
#define	MAX_FILE_LEN			(65024 * 16128)

#define	FILE_MODE_READ		0x01
#define	FILE_MODE_WRITE		0x02
#define	FILE_MODE_APPEND	0x04
#define	FILE_MODE_ALL		(FILE_MODE_READ | FILE_MODE_WRITE | FILE_MODE_APPEND)

typedef struct File
{
	char symbol[3];
	uint file_block_id;
	struct FileBlock * file_block;
	int mode;
	uint next_block_index;
	uint next_block_pos;
} IFS1_FILE;

extern int format_disk(char * symbol);
extern int check_disk(char * symbol);
extern uint parse_path(char * path, char * symbol, int * type);
extern int df_count(char * path);
extern int df(char * path, struct RawBlock * blocks);
extern int create_dir(char * path, char * name);
extern int create_file(char * path, char * name);
extern int del_dir(char * path);
extern int del_file(char * path);
extern int del_dirs(char * path);
extern int prev_dir(char * path, char * new_path);
extern int file_dir(char * path, char * new_patn);
extern int is_valid_df_name(char * name);
extern int rename_dir(char * path, char * new_name);
extern int rename_file(char * path, char * new_name);
extern int fix_path(char * path, char * current_path, char * new_path);
extern int is_sub_dir(char * dir, char * sub_dir);
extern int copy_file(char * src_path, char * dst_path, char * dst_name);



extern int exists_dir(char * path, char * name);
extern int exists_file(char * path, char * name);
extern int exists_df(char * path);
extern IFS1_FILE * IFS1_fopen(char * path, int mode);
extern int IFS1_fclose(IFS1_FILE * fptr);
extern int IFS1_fwrite(IFS1_FILE * fptr, uchar * buffer, uint len);
extern uint IFS1_fread(IFS1_FILE * fptr, uchar * buffer, uint len);
extern void IFS1_freset(IFS1_FILE * fptr);
extern int IFS1_fappend(IFS1_FILE * fptr, uchar * buffer, uint len);

//Test
extern int test_fappend(void);

#define	flen(fptr)	((fptr)->file_block->length)

#endif

